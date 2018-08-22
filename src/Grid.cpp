//
// Created by tommaso on 03/08/18.
//

#include <cstdlib>
#include <functional>
#include "Grid.h"

// NOTE: nice alloc and dealloc come from https://stackoverflow.com/a/1403157
void Grid::allocateGrid()
{
    int extendedRows = rows + 2;
    int extendedColumns = columns + 2;
    data = new CellData *[extendedRows];
    data[0] = new CellData[extendedRows * extendedColumns](); // "()" at the end ensure initialization to 0
    for (int i = 1; i < extendedRows; ++i)
    {
        data[i] = data[0] + i * extendedColumns;
    }
}

void Grid::deallocateGrid()
{
    delete[] data[0];
    delete[] data;
}

Grid::Grid(int columns, int rows, Logger &logger) : columns(columns),
                                    rows(rows),
                                    rowDistribution(1, rows),
                                    columnDistribution(1, columns),
                                    rowOffsetDistribution(-1, 1),
                                    columnOffsetDistribution(-1, 1),
                                    logger(logger),
                                    nextAvailableChainId(1)
{
    rowGenerator.seed(std::random_device()());
    columnGenerator.seed(std::random_device()());
    rowOffsetGenerator.seed(std::random_device()());
    columnOffsetGenerator.seed(std::random_device()());
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
    return rowDistribution(rowGenerator);
}

inline int Grid::pickColumn()
{
    return columnDistribution(columnGenerator);
}

void Grid::pickRandomElement(int &i, int &j)
{
    i = pickColumn();
    j = pickRow();
}

inline int Grid::pickRowOffset()
{
    return rowOffsetDistribution(rowOffsetGenerator);
}

inline int Grid::pickColumnOffset()
{
    return columnOffsetDistribution(columnOffsetGenerator);
}

void Grid::pickRandomNeighbourOf(int i, int j, int &neighbourI, int &neighbourJ)
{
    do
    {
        neighbourI = i + pickColumnOffset();
        neighbourJ = j + pickRowOffset();
    } while (
            (neighbourI == i && neighbourJ == j) // We want a neighbour, not the cell itself
            || neighbourI < 1 || neighbourI > columns // We want to be inside the grid
            || neighbourJ < 1 || neighbourJ > rows // We want to be inside the grid
            );
}

int Grid::initializeInnerGridAs(ChemicalSpecies chemicalSpecies, Activity activity)
{
    for (int j = getFirstRow(); j <= getLastRow(); ++j)
    {
        for (int i = getFirstColumn(); i <= getLastColumn(); ++i)
        {
            setChemicalProperties(i, j, chemicalSpecies, activity);
        }
    }
    return columns*rows;
}

int Grid::initializeGridRandomly(double randomRatio, ChemicalSpecies chemicalSpecies, Activity activity)
{
    logger.logMsg(PRODUCTION, "Initializing grid randomly: %s=%f", DUMP(randomRatio));
    
    int numRandomCells = static_cast<int>(round(columns * rows * randomRatio));
    int i = 0;
    int safetyCounter = 0;
    int maxIter = 5 * columns * rows;
    while (i < numRandomCells && safetyCounter < maxIter)
    {
        int x = 0, y = 0;
        pickRandomElement(x, y);
        if (chainsCellBelongsTo(x, y).empty())
        {
            setChemicalProperties(x, y, chemicalSpecies, activity);
            ++i;
        }
        ++safetyCounter;
    }
    return i;
}

int Grid::initializeGridWithSingleChain(ChemicalSpecies chemicalSpecies, Activity activity)
{
    logger.logMsg(PRODUCTION, "Initializing grid with single horizontal chain");
    
    int chainCol = columns / 2;
    unsigned short chainId = getNewChainId();
    for (int j = 1; j <= rows; ++j)
    {
        setChemicalProperties(chainCol, j, chemicalSpecies, activity);
        setChainProperties(chainCol, j, chainId,
                           static_cast<unsigned int>(j - 1),
                           (unsigned int) rows);
    }
    return rows;
}

int Grid::initializeGridWithTwoParallelChains(int distance, ChemicalSpecies chemicalSpecies, Activity activity)
{
    logger.logMsg(PRODUCTION, "Initializing grid with two parallel chains: %s=%d", DUMP(distance));
    
    int chainCol = (columns + distance) / 2;
    unsigned short chainId1 = getNewChainId();
    unsigned short chainId2 = getNewChainId();
    for (int j = 1; j <= rows; ++j)
    {
        setChemicalProperties(chainCol, j, chemicalSpecies, activity);
        setChainProperties(chainCol, j, chainId1,
                           static_cast<unsigned int>(j - 1),
                           (unsigned int) rows);
        setChemicalProperties(chainCol - distance, j, chemicalSpecies, activity);
        setChainProperties(chainCol - distance, j, chainId2,
                           static_cast<unsigned int>(j - 1),
                           (unsigned int) rows);
    }
    return 2*rows;
}

int Grid::initializeGridWithTwoOrthogonalChains(int xOffset, int yOffset, ChemicalSpecies chemicalSpecies, Activity activity)
{
    logger.logMsg(PRODUCTION, "Initializing grid with two orthogonal chains: %s=%d, %s=%d", DUMP(xOffset), DUMP(yOffset));
    
    int chainCol = (columns / 2) + xOffset;
    int chainRow = (rows / 2) + yOffset;
    unsigned short chainId1 = getNewChainId();
    for (int j = 1; j <= rows; ++j)
    {
        setChemicalProperties(chainCol, j, chemicalSpecies, activity);
        setChainProperties(chainCol, j, chainId1,
                           static_cast<unsigned int>(j - 1),
                           (unsigned int) rows);
    }
    
    unsigned short chainId2 = getNewChainId();
    for (int i = 1; i <= columns; ++i)
    {
        setChemicalProperties(i, chainRow, chemicalSpecies, activity);
        setChainProperties(i, chainRow, chainId2,
                           static_cast<unsigned int>(i - 1),
                           (unsigned int) columns);
    }
    
    return rows + columns;
}

void Grid::setChemicalSpecies(ChemicalProperties &target, ChemicalSpecies species)
{
    setBit(target, SPECIES_BIT, species);
}

void Grid::setChemicalSpecies(int column, int row, ChemicalSpecies species)
{
    setChemicalSpecies(getElement(column, row).chemicalProperties, species);
}

void Grid::setActivity(ChemicalProperties &target, Activity activity)
{
    setBit(target, ACTIVE_BIT, activity);
}

void Grid::setActivity(int column, int row, Activity activity)
{
    setActivity(getElement(column, row).chemicalProperties, activity);
}

void Grid::setChemicalProperties(ChemicalProperties &target, ChemicalSpecies species, Activity activity)
{
    setChemicalSpecies(target, species);
    setActivity(target, activity);
}

void Grid::setChemicalProperties(int column, int row, ChemicalSpecies species, Activity activity)
{
    setChemicalProperties(getElement(column, row).chemicalProperties, species, activity);
}

void Grid::setChemicalProperties(int column, int row, ChemicalProperties chemicalProperties)
{
    setChemicalProperties(column, row, static_cast<ChemicalSpecies>(getBit(chemicalProperties, SPECIES_BIT)),
                          static_cast<Activity>(getBit(chemicalProperties, ACTIVE_BIT)));
}

ChemicalProperties Grid::chemicalPropertiesOf(ChemicalSpecies species, Activity activity)
{
    ChemicalProperties chemicalProperties = 0;
    setChemicalProperties(chemicalProperties, species, activity);
    return chemicalProperties;
}

void Grid::setFlags(int column, int row, Flags flags)
{
    getElement(column, row).flags = flags;
}

void Grid::setTranscribability(int column, int row, Transcribability transcribability)
{
    setBit(getElement(column, row).flags, TRANSCRIBABLE_BIT, transcribability);
}

CellData &Grid::getElement(int i, int j)
{
    return data[j][i];
}

CellData &Grid::getElement(int i, int j) const
{
    return data[j][i];
}

void Grid::setElement(int i, int j, CellData &value)
{
    data[j][i] = value;
}

size_t Grid::setChainProperties(int column, int row, unsigned short chainId, unsigned int position, unsigned int length)
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

unsigned char Grid::cellBelongsToChain(int column, int row, unsigned short chainId)
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

bool Grid::isCellNeighbourInAnyChain(int column, int row, std::vector<std::reference_wrapper<ChainProperties>> &chainPropertiesVector)
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
        ChainProperties &chainProperties = data[row][column].chainProperties[k];
        unsigned int chainLength = chainProperties.chainLength;
        if (chainLength > 0)
        {
            chains.push_back(std::ref(chainProperties));
        }
    }
    return chains;
}

unsigned short Grid::getNewChainId()
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
