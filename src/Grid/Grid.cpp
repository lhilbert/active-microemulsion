#include <utility>

//
// Created by tommaso on 03/08/18.
//

#include <cstdlib>
#include <functional>
#include "Grid.h"
#include "../Utils/RandomGenerator.h"

std::mt19937 Grid::randomNumberGenerator = RandomGenerator::getInstance().getGenerator();

// NOTE: nice alloc and dealloc come from https://stackoverflow.com/a/1403157
void Grid::allocateGrid()
{
    int extendedRows = rows + 2;
    int extendedColumns = columns + 2;
    data = new CellData *[extendedRows];
    data[0] = new CellData[extendedRows * extendedColumns](); // "()" at the end ensure initialization to 0
//    #pragma omp parallel for schedule(static) //first touch on grid
    for (int i = 1; i < extendedRows - 1; ++i)
    {
        data[i] = data[0] + i * extendedColumns;
//        memset(data[i], 0, static_cast<size_t>(extendedColumns));
    }
    data[extendedRows - 1] = data[0] + (extendedRows - 1) * extendedColumns;
}

void Grid::deallocateGrid()
{
    delete[] data[0];
    delete[] data;
}

Grid::Grid(int columns, int rows, Logger &logger) : columns(columns),
                                                    rows(rows),
                                                    numElements(columns * rows),
                                                    rowDistribution(1, rows),
                                                    columnDistribution(1, columns),
                                                    elementDistribution(0, numElements-1),
                                                    rowColOffsetDistribution(0, 8),
                                                    logger(logger),
                                                    nextAvailableChainId(1)
{
    #pragma omp parallel for schedule(static,1)
    for (int i=0; i < omp_get_num_threads(); ++i)
    {
        randomNumberGenerator = RandomGenerator::getInstance().getGenerator();
    }
    allocateGrid();
}

Grid::~Grid()
{
    deallocateGrid();
}

const CellData **Grid::getData()
{
    return const_cast<const CellData **>(data);
}

inline int Grid::pickRow()
{
    return rowDistribution(randomNumberGenerator);
}

inline int Grid::pickColumn()
{
    return columnDistribution(randomNumberGenerator);
}

void Grid::pickRandomElement(int &i, int &j)
{
    int elementId = elementDistribution(randomNumberGenerator);
    i = 1 + (elementId % getColumns());
    j = 1 + (elementId / getColumns());
}

inline int Grid::pickRowColOffset()
{
    return rowColOffsetDistribution(randomNumberGenerator);
}

void Grid::pickNeighbourOffsets(int &rowOffset, int &colOffset)
{
    int rowColOffset = pickRowColOffset();
    rowOffset = (rowColOffset / 3) - 1;
    colOffset = (rowColOffset % 3) - 1;
}

void Grid::pickRandomNeighbourOf(int i, int j, int &neighbourI, int &neighbourJ)
{
    do
    {
        int rowOffset, colOffset;
        pickNeighbourOffsets(rowOffset, colOffset);
        neighbourI = i + colOffset;
        neighbourJ = j + rowOffset;
    } while (
            (neighbourI == i && neighbourJ == j) // We want a neighbour, not the cell itself
            || neighbourI < 1 || neighbourI > columns // We want to be inside the grid
            || neighbourJ < 1 || neighbourJ > rows // We want to be inside the grid
            );
}

void Grid::initializeCellProperties(int column, int row, ChemicalProperties chemicalProperties, Flags flags,
                                    bool enforceChainIntegrity, ChainId chainId, unsigned int chainLength,
                                    unsigned int position)
{
    setChemicalProperties(column, row, chemicalProperties);
    setFlags(column, row, flags);
    if (enforceChainIntegrity)
    {
        setChainProperties(column, row, chainId, position, chainLength);
    }
}

void Grid::setChemicalSpecies(int column, int row, ChemicalSpecies species)
{
    getElement(column, row).setChemicalSpecies(species);
}

void Grid::setActivity(int column, int row, Activity activity)
{
    getElement(column, row).setActivity(activity);
}

void Grid::setChemicalProperties(int column, int row, ChemicalSpecies species, Activity activity)
{
    getElement(column, row).setChemicalProperties(species, activity);
}

void Grid::setChemicalProperties(int column, int row, ChemicalProperties chemicalProperties)
{
    setChemicalProperties(column, row,
                          static_cast<ChemicalSpecies>(BitwiseOperations::getBit(chemicalProperties, SPECIES_BIT)),
                          static_cast<Activity>(BitwiseOperations::getBit(chemicalProperties, ACTIVE_BIT)));
}

void Grid::setFlags(int column, int row, Flags flags)
{
    getElement(column, row).flags = flags;
}

void Grid::setTranscribability(int column, int row, Transcribability transcribability)
{
    getElement(column, row).setTranscribability(transcribability);
}

void Grid::setTranscriptionInhibition(int column, int row, TranscriptionInhibition inhibition)
{
    getElement(column, row).setTranscriptionInhibition(inhibition);
}

CellData &Grid::getElement(int elementId)
{
    return data[0][elementId]; // data[0] is the pointer to the array storing the entire grid in 1D
}

CellData &Grid::getElement(int column, int row)
{
    return data[row][column];
}

CellData &Grid::getElement(int column, int row) const
{
    return data[row][column];
}

void Grid::setElement(int column, int row, CellData &value)
{
    data[row][column] = value;
}

size_t Grid::setChainProperties(int column, int row, ChainId chainId, unsigned int position, unsigned int length)
{
    CellData &element = getElement(column, row);
    int k = 0;
    while (element.chainProperties[k].chainLength != 0 && k < MAX_CROSSING_CHAINS)
    {
        ++k;
    }
    if (k == MAX_CROSSING_CHAINS)
    {
        logger.logMsg(ERROR, "Trying to get too many chains to cross!");
        logger.flush(); // Flushing before everything explodes...
        throw std::out_of_range("Trying to get too many chains to cross");
    }
    
    element.chainProperties[k].chainId = chainId;
    element.chainProperties[k].position = position;
    element.chainProperties[k].chainLength = length;
    
    return static_cast<size_t>(k);
}

unsigned char Grid::cellBelongsToChain(int column, int row, ChainId chainId)
{
    unsigned char found = MAX_CROSSING_CHAINS;
    for (unsigned char k = 0; k < MAX_CROSSING_CHAINS; ++k)
    {
        if (getElement(column, row).chainProperties[k].chainId == chainId)
        {
            found = k;
            break;
        }
    }
    return found;
}

bool Grid::isPositionInChainPropertiesArrayValid(unsigned char position)
{
    return position < MAX_CROSSING_CHAINS;
}

bool Grid::isCellNeighbourInChain(int column, int row, const ChainProperties &chainProperties)
{
    
    bool isNeighbourInChain = false;
    // First check if the given cell belongs to the given chain
    unsigned char pos = cellBelongsToChain(column, row, chainProperties.chainId);
    if (isPositionInChainPropertiesArrayValid(pos))
    {
        // Then check if it is actually a neighbour within the given chain
        unsigned int neighbourPositionInChain =
                getElement(column, row).chainProperties[pos].position;
        unsigned int currentCellPositionInChain = chainProperties.position;
        int distance = neighbourPositionInChain - currentCellPositionInChain;
        isNeighbourInChain = (distance == 1) || (distance == -1);
    }
    return isNeighbourInChain;
}

bool Grid::isCellNeighbourInAnyChain(int column, int row,
                                     std::vector<std::reference_wrapper<ChainProperties>> &chainPropertiesVector)
{
    bool isNeighbour = false;
    for (auto chain : chainPropertiesVector)
    {
        if (isCellNeighbourInChain(column, row, chain))
        {
            isNeighbour = true;
            break;
        }
    }
    return isNeighbour;
}

std::vector<std::reference_wrapper<ChainProperties>> Grid::chainsCellBelongsTo(int column, int row)
{
    auto chains = std::vector<std::reference_wrapper<ChainProperties>>();
    for (unsigned char k = 0; k < MAX_CROSSING_CHAINS; ++k)
    {
        ChainProperties &chainProperties = getElement(column, row).chainProperties[k];
        unsigned int chainLength = chainProperties.chainLength;
        if (chainLength > 0)
        {
            chains.push_back(std::ref(chainProperties));
        }
    }
    return chains;
}

ChainId Grid::getNewChainId()
{
    logger.logMsg(PRODUCTION, "Initializing new chain with chainId=%d", nextAvailableChainId);
    return nextAvailableChainId++; // Return and increment
}

int Grid::getSpeciesCount(ChemicalSpecies chemicalSpecies)
{
    int counter = 0;
    for (int j = getFirstRow(); j <= getLastRow(); ++j)
    {
        for (int i = getFirstColumn(); i <= getLastColumn(); ++i)
        {
            counter += (getChemicalSpecies(i, j) == chemicalSpecies);
        }
    }
    return counter;
}

bool Grid::doesAnyNeighbourMatchCondition(int column, int row, bool (*condition)(CellData &))
{
    return condition(getElement(column - 1, row - 1))
           || condition(getElement(column, row - 1))
           || condition(getElement(column + 1, row - 1))
           || condition(getElement(column - 1, row))
           || condition(getElement(column + 1, row))
           || condition(getElement(column - 1, row + 1))
           || condition(getElement(column, row + 1))
           || condition(getElement(column + 1, row + 1));
}

std::vector<std::reference_wrapper<CellData>>
Grid::getNeighboursMatchingConditions(int column, int row, bool (*condition)(CellData &))
{
    auto neighboursMatchingCondition = std::vector<std::reference_wrapper<CellData>>();
    for (int colOffset = -1; colOffset <= 1; ++colOffset)
    {
        for (int rowOffset = -1; rowOffset <= 1; ++rowOffset)
        {
            if (!(colOffset == 0 && rowOffset == 0))
            {
                CellData &neighbour = getElement(column + colOffset, row + rowOffset);
                if (condition(neighbour))
                {
                    neighboursMatchingCondition.push_back(neighbour);
                }
            }
        }
    }
    return neighboursMatchingCondition;
}

bool Grid::isPositionNextToBoundary(int column, int row)
{
    return column == getFirstColumn() || column == getLastColumn()
           || row == getFirstRow() || row == getLastRow();
}

void Grid::walkOnGrid(int &column, int &row, signed char colOffset, signed char rowOffset)
{
    column += colOffset;
    row += rowOffset;
}

bool Grid::isCellWithinInternalDomain(int column, int row)
{
    return column >= getFirstColumn()
           && column <= getLastColumn()
           && row >= getFirstRow()
           && row <= getLastRow();
}

int Grid::getColumns() const
{
    return columns;
}

int Grid::getRows() const
{
    return rows;
}

