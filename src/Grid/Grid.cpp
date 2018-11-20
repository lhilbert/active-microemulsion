#include <utility>

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
    genericGenerator.seed(std::random_device()());
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

//std::set<ChainId> Grid::initializeGridWith3WaySegmentedChain(int offsetFromCenter, int startColumn, int endColumn,
//                                                             ChemicalProperties chemicalPropertiesA,
//                                                             ChemicalProperties chemicalPropertiesB,
//                                                             Flags flags,
//                                                             bool enforceChainIntegrity)
//{
////    logger.logMsg(PRODUCTION, "Initializing grid with horizontal 3way-segmented chain");
//    std::set<ChainId> newChains;
////
////    int chainRow = (rows / 2) + offsetFromCenter;
////    ChainId chainId1 = 0;
////    ChainId chainId2 = 0;
////    ChainId chainId3 = 0;
////    if (enforceChainIntegrity)
////    {
////        chainId1 = getNewChainId();
////        newChains.insert(chainId1);
////        chainId2 = getNewChainId();
////        newChains.insert(chainId2);
////        chainId3 = getNewChainId();
////        newChains.insert(chainId3);
////    }
////
////    int le
////    for (int column = 1; column <= columns; ++column)
////    {
////        setChemicalProperties(column, chainRow, chemicalProperties);
////        setFlags(column, chainRow, flags);
////        if (enforceChainIntegrity)
////        {
////            setChainProperties(column, chainRow, chainId1,
////                               static_cast<unsigned int>(column - 1),
////                               (unsigned int) rows);
////        }
////    }
//    return newChains;
//}

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

void Grid::setFlags(Flags &target, Transcribability transcribability, TranscriptionInhibition inhibition)
{
    setTranscribability(target, transcribability);
    setTranscriptionInhibition(target, inhibition);
}

void Grid::setFlags(int column, int row, Flags flags)
{
    getElement(column, row).flags = flags;
}

void Grid::setTranscribability(Flags &target, Transcribability transcribability)
{
    setBit(target, TRANSCRIBABLE_BIT, transcribability);
}

void Grid::setTranscribability(int column, int row, Transcribability transcribability)
{
    setTranscribability(getElement(column, row).flags, transcribability);
}

void Grid::setTranscriptionInhibition(Flags &target, TranscriptionInhibition inhibition)
{
    setBit(target, TRANSCRIPTION_INHIBITION_BIT, inhibition);
}

void Grid::setTranscriptionInhibition(int column, int row, TranscriptionInhibition inhibition)
{
    setTranscriptionInhibition(getElement(column, row).flags, inhibition);
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

Flags Grid::flagsOf(Transcribability transcribability, TranscriptionInhibition inhibition)
{
    Flags flags = 0;
    setFlags(flags, transcribability, inhibition);
    return flags;
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

std::set<ChainId> Grid::chainsCellBelongsTo(CellData &cellData)
{
    auto chainIdSet = std::set<ChainId>();
    for (unsigned char k = 0; k < MAX_CROSSING_CHAINS; ++k)
    {
        ChainProperties &chainProperties = cellData.chainProperties[k];
        unsigned int chainLength = chainProperties.chainLength;
        if (chainLength > 0)
        {
            chainIdSet.insert(chainProperties.chainId);
        }
    }
    return chainIdSet;
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
