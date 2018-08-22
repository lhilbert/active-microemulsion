//
// Created by tommaso on 03/08/18.
//

#ifndef ACTIVE_MICROEMULSION_GRID_H
#define ACTIVE_MICROEMULSION_GRID_H

#include <random>
#include <functional>
#include "CellData.h"
#include "Logger.h"

/*
 * The Grid class represents a discretized domain with a regular grid.
 * It supports the allocation/destruction of the matrix, it retains the grid properties (e.g. index ranges) and
 * it has a method for getting a random (uniformly distributed) element of the domain.
 */
class Grid
{
public:
    // Columns and rows are the values of the inner number of rows and columns, without the external halo.
    const int columns, rows;
private:
    CellData **data;
    std::mt19937_64 rowGenerator, columnGenerator,
            rowOffsetGenerator, columnOffsetGenerator;
    std::uniform_int_distribution<int> rowDistribution, columnDistribution,
            rowOffsetDistribution, columnOffsetDistribution;
    Logger &logger;
    unsigned short nextAvailableChainId;

public:
    Grid(int columns, int rows, Logger &logger);
    
    ~Grid();
    
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
    
    const CellData **getData();
    
    CellData &getElement(int i, int j);
    
    CellData &getElement(int i, int j) const;
    
    void setElement(int i, int j, CellData &value);
    
    int initializeInnerGridAs(ChemicalSpecies chemicalSpecies, Activity activity);
    
    int initializeGridRandomly(double randomRatio, ChemicalSpecies chemicalSpecies, Activity activity);
    
    int initializeGridWithSingleChain(ChemicalSpecies chemicalSpecies, Activity activity);
    
    int initializeGridWithTwoParallelChains(int distance, ChemicalSpecies chemicalSpecies, Activity activity);
    
    int initializeGridWithTwoOrthogonalChains(int xOffset, int yOffset, ChemicalSpecies chemicalSpecies, Activity activity);
    
    void pickRandomElement(int &i, int &j);
    
    void pickRandomNeighbourOf(int i, int j, int &neighbourI, int &neighbourJ);
    
    static inline ChemicalProperties getChemicalProperties(CellData &cellData)
    {
        return cellData.chemicalProperties;
    }
    
    inline ChemicalProperties getChemicalProperties(int column, int row) const
    {
        return getChemicalProperties(getElement(column, row));
    }
    
    static inline ChemicalSpecies getChemicalSpecies(CellData &cellData)
    {
        return static_cast<ChemicalSpecies>((getChemicalProperties(cellData) >> SPECIES_BIT) & 1U);
    }
    
    inline ChemicalSpecies getChemicalSpecies(int column, int row) const
    {
        return static_cast<ChemicalSpecies>((getChemicalProperties(column, row) >> SPECIES_BIT) & 1U);
    }
    
    static inline bool isChromatine(CellData &cellData)
    {
        return getChemicalSpecies(cellData) == CHROMATINE;
    }
    
    static inline bool isRBP(CellData &cellData)
    {
        return getChemicalSpecies(cellData) == RBP;
    }
    
    static inline bool isActive(CellData &cellData)
    {
        return ((getChemicalProperties(cellData) >> ACTIVE_BIT) & 1U) == ACTIVE;
    }
    
    static inline Flags getFlags(CellData &cellData)
    {
        return cellData.flags;
    }
    
    inline Flags getFlags(int column, int row) const
    {
        return getFlags(getElement(column, row));
    }
    
    static inline bool isTranscribable(CellData &cellData)
    {
        return ((getFlags(cellData) >> TRANSCRIBABLE_BIT) & 1U) == TRANSCRIBABLE;
    }
    
    // This is used to check if a swap is meaningless, however this must not include a chain check!
    inline bool areCellsIndistinguishable(int column, int row,
                                          int nColumn, int nRow) const
    {
        CellData &cellData = getElement(column, row);
        CellData &nCellData = getElement(nColumn, nRow);
        return (getChemicalProperties(cellData) == getChemicalProperties(nCellData))
            && (getFlags(cellData) == getFlags(nCellData));
    }
    
    int getSpeciesCount(ChemicalSpecies chemicalSpecies);
    
    static void setChemicalSpecies(ChemicalProperties &target, ChemicalSpecies species);
    void setChemicalSpecies(int column, int row, ChemicalSpecies species);
    
    static void setActivity(ChemicalProperties &target, Activity activity);
    void setActivity(int column, int row, Activity activity);
    
    static void setChemicalProperties(ChemicalProperties &target, ChemicalSpecies species, Activity activity);
    void setChemicalProperties(int column, int row, ChemicalSpecies species, Activity activity);
    void setChemicalProperties(int column, int row, ChemicalProperties chemicalProperties);
    
    static ChemicalProperties chemicalPropertiesOf(ChemicalSpecies species, Activity activity);
    
    void setFlags(int column, int row, Flags flags);
    
    void setTranscribability(int column, int row, Transcribability transcribability);
    
    // Set chain properties in the first slot available and return the index of the slot used
    size_t setChainProperties(int column, int row,
                              unsigned short chainId, unsigned int position, unsigned int length);
    
    // Check cell of given coordinates and return chains it belongs to
    std::vector<std::reference_wrapper<ChainProperties>> chainsCellBelongsTo(int column, int row);
    
    // Check if a given cell belongs to a chain with given Id
    // Returns position in chain properties array if found, MAX_CROSSING_CHAINS if not found
    unsigned char cellBelongsToChain(int column, int row, unsigned short chainId);
    
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

private:
    void allocateGrid();
    
    void deallocateGrid();
    
    inline int pickRow();
    
    inline int pickColumn();
    
    inline int pickRowOffset();
    
    inline int pickColumnOffset();
    
    inline bool isPositionInChainPropertiesArrayValid(unsigned char position);
    
    unsigned short getNewChainId();
    
    inline static bool getBit(unsigned char bitfield, unsigned char bit)
    {
        return static_cast<bool>((bitfield >> bit) & 1U);
    }
    
    inline static void setBit(unsigned char &bitfield, unsigned char bit, unsigned char value)
    {
        // Set the SPECIES_BIT to the species value
        bitfield ^= (-value ^ bitfield) & (1U << bit);
    }
};


#endif //ACTIVE_MICROEMULSION_GRID_H
