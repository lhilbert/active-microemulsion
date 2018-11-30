//
// Created by tommaso on 03/08/18.
//

#ifndef ACTIVE_MICROEMULSION_GRID_H
#define ACTIVE_MICROEMULSION_GRID_H

#include <random>
#include <functional>
#include <set>
#include <omp.h>
#include "../Cell/CellData.h"
#include "../Logger/Logger.h"

class GridInitializer;

/*
 * The Grid class represents a discretized domain with a regular grid.
 * It supports the allocation/destruction of the matrix, it retains the grid properties (e.g. index ranges) and
 * it has a method for getting a random (uniformly distributed) element of the domain.
 */
class Grid
{
    friend GridInitializer;
    
private:
//    const unsigned char dim = 2;
    // Columns and rows are the values of the inner number of rows and columns, without the external halo.
    const int columns, rows;
    static std::mt19937 randomNumberGenerator;
    #pragma omp threadprivate(randomNumberGenerator)
    int numElements;
    CellData **data;
    std::uniform_int_distribution<int> rowDistribution, columnDistribution, elementDistribution,
            rowColOffsetDistribution;
    Logger &logger;
    ChainId nextAvailableChainId;

public:
    Grid(int columns, int rows, Logger &logger);
    
    ~Grid();
    
    int getColumns() const;
    
    int getRows() const;
    
    inline int getFirstRow() const
    {
        return 1;
    }
    
    inline int getLastRow() const
    {
        return rows;
    }
    
    inline int getFirstColumn() const
    {
        return 1;
    }
    
    inline int getLastColumn() const
    {
        return columns;
    }
    
    bool isCellWithinInternalDomain(int column, int row);
    
    const CellData **getData();
    
    /**
     * Get an element reference by using a 1D id.
     * @param elementId
     * @return
     */
    CellData &getElement(int elementId);
 
    CellData &getElement(int column, int row);
    
    CellData &getElement(int column, int row) const;
    
    void setElement(int column, int row, CellData &value);
    
    void pickRandomElement(int &i, int &j);
    
    void pickRandomNeighbourOf(int i, int j, int &neighbourI, int &neighbourJ);
    
    inline ChemicalProperties getChemicalProperties(int column, int row) const
    {
        return getElement(column, row).getChemicalProperties();
    }
    
    inline ChemicalSpecies getChemicalSpecies(int column, int row) const
    {
        return CellData::getChemicalSpecies(getChemicalProperties(column, row));
    }
    
    inline bool isChromatin(int column, int row)
    {
        return getElement(column, row).isChromatin();
    }
    
    inline bool isInactiveChromatin(int column, int row)
    {
        return getElement(column, row).isInactiveChromatin();
    }
    
    inline Flags getFlags(int column, int row) const
    {
        return getElement(column, row).getFlags();
    }
    
    // This is used to check if a swap is meaningless, however this must not include a chain check!
    inline bool areCellsIndistinguishable(int column, int row,
                                          int nColumn, int nRow) const
    {
        CellData &cellData = getElement(column, row);
        CellData &nCellData = getElement(nColumn, nRow);
        return (cellData.getChemicalProperties() == nCellData.getChemicalProperties())
               && (cellData.getFlags() == nCellData.getFlags());
    }
    
    int getSpeciesCount(ChemicalSpecies chemicalSpecies);
    
    void setChemicalSpecies(int column, int row, ChemicalSpecies species);
    
    void setActivity(int column, int row, Activity activity);
    
    void setChemicalProperties(int column, int row, ChemicalSpecies species, Activity activity);
    
    void setChemicalProperties(int column, int row, ChemicalProperties chemicalProperties);
    
    void setFlags(int column, int row, Flags flags);
    
    void setTranscribability(int column, int row, Transcribability transcribability);
    
    void setTranscriptionInhibition(int column, int row, TranscriptionInhibition inhibition);
    
    // Set chain properties in the first slot available and return the index of the slot used
    size_t setChainProperties(int column, int row,
                              ChainId chainId, unsigned int position, unsigned int length);
    
    // Check cell of given coordinates and return chains it belongs to
    std::vector<std::reference_wrapper<ChainProperties>> chainsCellBelongsTo(int column, int row);
    
    // Check if a given cell belongs to a chain with given Id
    // Returns position in chain properties array if found, MAX_CROSSING_CHAINS if not found
    unsigned char cellBelongsToChain(int column, int row, ChainId chainId);
    
    // Check if the cell with given coordinates is neighbour (in the given chain)
    // of the cell having the given chain properties.
    bool isCellNeighbourInChain(int column, int row, const ChainProperties &chainProperties);
    
    // Check if the cell with given coordinates is neighbour (in any of the given chains)
    // of the cell having the given chain properties' vector.
    bool isCellNeighbourInAnyChain(int column, int row,
                                   std::vector<std::reference_wrapper<ChainProperties>> &chainPropertiesVector);
    
    // Check if given cell is last of given chain
    inline bool isLastOfChain(int column, int row, const ChainProperties &chain) const
    {
        return chain.position == chain.chainLength - 1; // 0-based indexing...
    }
    
    bool doesAnyNeighbourMatchCondition(int column, int row,
                                        bool (*condition)(CellData &));
    
    std::vector<std::reference_wrapper<CellData>>
    getNeighboursMatchingConditions(int column, int row, bool (*condition)(CellData &));
    
    bool isPositionNextToBoundary(int column, int row);

private:
    void allocateGrid();
    
    void deallocateGrid();
    
    inline int pickRow();

    inline int pickColumn();
    
    inline int pickRowColOffset();
    
    void pickNeighbourOffsets(int &rowOffset, int &colOffset);
    
    static void walkOnGrid(int &column, int &row, signed char colOffset, signed char rowOffset);
    
    inline bool isPositionInChainPropertiesArrayValid(unsigned char position);
    
    ChainId getNewChainId();
    
    void initializeCellProperties(int column, int row, ChemicalProperties chemicalProperties, Flags flags,
                                  bool enforceChainIntegrity, ChainId chainId, unsigned int chainLength,
                                  unsigned int position);
};


#endif //ACTIVE_MICROEMULSION_GRID_H
