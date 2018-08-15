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
                                    logger(logger)
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

int Grid::initializeGridRandomly(double randomRatio)
{
    logger.logMsg(PRODUCTION, "Initializing grid randomly: %s=%f", DUMP(randomRatio));
    
    int numRandomCells = static_cast<int>(round(columns * rows * randomRatio));
    for (int i = 0; i < numRandomCells; ++i)
    {
        int x = 0, y = 0;
        pickRandomElement(x, y);
        setSpecies(x, y, SPECIES_B);
    }
    return numRandomCells;
}

int Grid::initializeGridWithSingleChain()
{
    logger.logMsg(PRODUCTION, "Initializing grid with single horizontal chain");
    
    int chainCol = columns / 2;
    for (int j = 1; j <= rows; ++j)
    {
        setSpecies(chainCol, j, SPECIES_B);
        setChainProperties(chainCol, j, 1,
                static_cast<unsigned int>(j - 1),
                (unsigned int) rows);
    }
    return rows;
}

int Grid::initializeGridWithTwoParallelChains(int distance)
{
    logger.logMsg(PRODUCTION, "Initializing grid with two parallel chains: %s=%d", DUMP(distance));
    
    int chainCol = (columns + distance) / 2;
    for (int j = 1; j <= rows; ++j)
    {
        setSpecies(chainCol, j, SPECIES_B);
        setChainProperties(chainCol, j, 1,
                           static_cast<unsigned int>(j - 1),
                           (unsigned int) rows);
        setSpecies(chainCol - distance, j, SPECIES_B);
        setChainProperties(chainCol - distance, j, 2,
                           static_cast<unsigned int>(j - 1),
                           (unsigned int) rows);
    }
    return 2*rows;
}

int Grid::initializeGridWithTwoOrthogonalChains(int xOffset, int yOffset)
{
    logger.logMsg(PRODUCTION, "Initializing grid with two orthogonal chains: %s=%d, %s=%d", DUMP(xOffset), DUMP(yOffset));
    
    int chainCol = (columns / 2) + xOffset;
    int chainRow = (rows / 2) + yOffset;
    
    for (int j = 1; j <= rows; ++j)
    {
        setSpecies(chainCol, j, SPECIES_B);
        setChainProperties(chainCol, j, 1,
                           static_cast<unsigned int>(j - 1),
                           (unsigned int) rows);
    }
    
    for (int i = 1; i <= columns; ++i)
    {
        setSpecies(i, chainRow, SPECIES_B);
        setChainProperties(i, chainRow, 2,
                           static_cast<unsigned int>(i - 1),
                           (unsigned int) columns);
    }
    
    return rows + columns;
}

void Grid::setSpecies(int column, int row, SpeciesBit species)
{
    // Set the SPECIES_BIT to the species value
    data[row][column].chemicalSpecies ^=
            (-species ^ data[row][column].chemicalSpecies) & (1U << SPECIES_BIT);
}

CellData &Grid::getElement(int i, int j)
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
