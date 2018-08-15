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
    const int columns, rows;
private:
    CellData **data;
    std::mt19937_64 rowGenerator, columnGenerator,
            rowOffsetGenerator, columnOffsetGenerator;
    std::uniform_int_distribution<int> rowDistribution, columnDistribution,
            rowOffsetDistribution, columnOffsetDistribution;
    Logger &logger;

public:
    Grid(int columns, int rows, Logger &logger);
    
    ~Grid();
    
    const CellData **getData();
    
    CellData &getElement(int i, int j);
    
    void setElement(int i, int j, CellData &value);
    
    int initializeGridRandomly(double randomRatio);
    
    int initializeGridWithSingleChain();
    
    int initializeGridWithTwoParallelChains(int distance);

    int initializeGridWithTwoOrthogonalChains(int xOffset, int yOffset);
    
    void pickRandomElement(int &i, int &j);
    
    void pickRandomNeighbourOf(int i, int j, int &neighbourI, int &neighbourJ);
    
    static inline bool isSpeciesA(CellData &cellData)
    {
        return static_cast<bool>((cellData.chemicalSpecies >> SPECIES_BIT) & 1) == SPECIES_A;
    }
    
    static inline bool isSpeciesB(CellData &cellData)
    {
        return static_cast<bool>((cellData.chemicalSpecies >> SPECIES_BIT) & 1) == SPECIES_B;
    }
    
    inline SpeciesBit getSpecies(int column, int row)
    {
        return static_cast<SpeciesBit>((data[row][column].chemicalSpecies >> SPECIES_BIT) & 1);
    }
    
    // This is used to check if a swap is meaningless, however this must not include a chain check!
    inline bool areCellsChemicallyIndistinguishable(int column, int row,
                                                    int nColumn, int nRow)
    {
        return data[row][column].chemicalSpecies == data[nRow][nColumn].chemicalSpecies;
    }
    
    void setSpecies(int column, int row, SpeciesBit species);
    
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
    bool isCellNeighbourInAnyChain(int column, int row, std::vector<std::reference_wrapper<ChainProperties>> &chainPropertiesVector);
    
    // Check if given cell is last of given chain
    inline bool isLastOfChain(int column, int row, const ChainProperties &chain)
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
};


#endif //ACTIVE_MICROEMULSION_GRID_H
