//
// Created by tommaso on 03/08/18.
//

#ifndef ACTIVE_MICROEMULSION_GRID_H
#define ACTIVE_MICROEMULSION_GRID_H

#include <random>
#include <functional>
#include <set>
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
    
public:
    // Columns and rows are the values of the inner number of rows and columns, without the external halo.
    const int columns, rows;
private:
    CellData **data;
    std::mt19937_64 genericGenerator,
            rowGenerator, columnGenerator,
            rowOffsetGenerator, columnOffsetGenerator;
    std::uniform_int_distribution<int> rowDistribution, columnDistribution,
            rowOffsetDistribution, columnOffsetDistribution;
    Logger &logger;
    ChainId nextAvailableChainId;

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
    
    bool isCellWithinInternalDomain(int column, int row);
    
    const CellData **getData();
    
    CellData &getElement(int column, int row);
    
    CellData &getElement(int column, int row) const;
    
    void setElement(int column, int row, CellData &value);
    
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
    
    static inline ChemicalSpecies getChemicalSpecies(ChemicalProperties chemicalProperties)
    {
        return static_cast<ChemicalSpecies>((chemicalProperties >> SPECIES_BIT) & 1U);
    }
    
    static inline ChemicalSpecies getChemicalSpecies(CellData &cellData)
    {
        return getChemicalSpecies(getChemicalProperties(cellData));
    }
    
    inline ChemicalSpecies getChemicalSpecies(int column, int row) const
    {
        return getChemicalSpecies(getChemicalProperties(column, row));
    }
    
    static inline bool isChromatin(ChemicalProperties chemicalProperties)
    {
        return getChemicalSpecies(chemicalProperties) == CHROMATIN;
    }
    
    static inline bool isChromatin(CellData &cellData)
    {
        return isChromatin(getChemicalProperties(cellData));
    }
    
    inline bool isChromatin(int column, int row)
    {
        return isChromatin(getElement(column, row));
    }
    
    static inline bool isRBP(ChemicalProperties chemicalProperties)
    {
        return getChemicalSpecies(chemicalProperties) == RBP;
    }
    
    static inline bool isRBP(CellData &cellData)
    {
        return isRBP(getChemicalProperties(cellData));
    }
    
    static inline bool isActive(ChemicalProperties chemicalProperties)
    {
        return ((chemicalProperties >> ACTIVE_BIT) & 1U) == ACTIVE;
    }
    
    static inline bool isActive(CellData &cellData)
    {
        return isActive(getChemicalProperties(cellData));
    }
    
    static inline bool isActiveChromatin(CellData &cellData)
    {
        return isActiveChromatin(getChemicalProperties(cellData));
    }
    
    static inline bool isActiveChromatin(ChemicalProperties chemicalProperties)
    {
        return isChromatin(chemicalProperties) && isActive(chemicalProperties);
    }
    
    static inline bool isInactiveChromatin(ChemicalProperties chemicalProperties)
    {
        return isChromatin(chemicalProperties) && !isActive(chemicalProperties);
    }
    
    inline bool isInactiveChromatin(int column, int row)
    {
        return isInactiveChromatin(getElement(column, row).chemicalProperties);
    }
    
    static inline bool isActiveRBP(CellData &cellData)
    {
        return isActiveRBP(getChemicalProperties(cellData));
    }
    
    static inline bool isActiveRBP(ChemicalProperties chemicalProperties)
    {
        return isRBP(chemicalProperties) && isActive(chemicalProperties);
    }
    
    static inline bool isInactiveRBP(ChemicalProperties chemicalProperties)
    {
        return isRBP(chemicalProperties) && !isActive(chemicalProperties);
    }
    
    static inline RnaCounter getRnaContent(CellData &cellData)
    {
        return cellData.rnaContent;
    }
    
    static inline void incrementRnaContent(CellData &cellData, RnaCounter amount=1)
    {
        //todo: Here include an anti-overflow check!
        cellData.rnaContent += amount;
    }
    
    static inline void decrementRnaContent(CellData &cellData, RnaCounter amount=1)
    {
        //todo: Here include an anti-underflow check!
        cellData.rnaContent -= amount;
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
    
    static inline bool isTranscriptionInhibited(CellData &cellData)
    {
        return ((getFlags(cellData) >> TRANSCRIPTION_INHIBITION_BIT) & 1U) == TRANSCRIPTION_INHIBITED;
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
    
    static void setFlags(Flags &target, Transcribability transcribability, TranscriptionInhibition inhibition);
    
    void setFlags(int column, int row, Flags flags);
    
    static void setTranscribability(Flags &target, Transcribability transcribability);
    
    void setTranscribability(int column, int row, Transcribability transcribability);
    
    static void setTranscriptionInhibition(Flags &target, TranscriptionInhibition inhibition);
    
    void setTranscriptionInhibition(int column, int row, TranscriptionInhibition inhibition);
    
    static Flags flagsOf(Transcribability transcribability, TranscriptionInhibition inhibition);
    
    // Set chain properties in the first slot available and return the index of the slot used
    size_t setChainProperties(int column, int row,
                              ChainId chainId, unsigned int position, unsigned int length);
    
    // Check cell of given coordinates and return chains it belongs to
    std::vector<std::reference_wrapper<ChainProperties>> chainsCellBelongsTo(int column, int row);
    
    std::set<ChainId> chainsCellBelongsTo(CellData &cellData);
    
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
    
    inline int pickRowOffset();
    
    inline int pickColumnOffset();
    
    static void walkOnGrid(int &column, int &row, signed char colOffset, signed char rowOffset);
    
    inline bool isPositionInChainPropertiesArrayValid(unsigned char position);
    
    ChainId getNewChainId();
    
    inline static bool getBit(unsigned char bitfield, unsigned char bit)
    {
        return static_cast<bool>((bitfield >> bit) & 1U);
    }
    
    inline static void setBit(unsigned char &bitfield, unsigned char bit, unsigned char value)
    {
        // Set the SPECIES_BIT to the species value
        bitfield ^= (-value ^ bitfield) & (1U << bit);
    }
    
    void initializeCellProperties(int column, int row, ChemicalProperties chemicalProperties, Flags flags,
                                  bool enforceChainIntegrity, ChainId chainId, unsigned int chainLength,
                                  unsigned int position);
};


#endif //ACTIVE_MICROEMULSION_GRID_H
