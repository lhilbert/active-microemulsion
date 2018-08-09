//
// Created by tommaso on 03/08/18.
//

#ifndef ACTIVE_MICROEMULSION_GRID_H
#define ACTIVE_MICROEMULSION_GRID_H

#include <random>

typedef enum {SPECIES_BIT=0} DataBitMask;
typedef enum {SPECIES_A=0, SPECIES_B=1} SpeciesBit;

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
    unsigned int **data;
//    std::default_random_engine rowGenerator, columnGenerator,
//                                rowOffsetGenerator, columnOffsetGenerator;
    std::mt19937_64 rowGenerator, columnGenerator,
                        rowOffsetGenerator, columnOffsetGenerator;
    std::uniform_int_distribution<int> rowDistribution, columnDistribution,
                                rowOffsetDistribution, columnOffsetDistribution;
    inline int pickRow();
    inline int pickColumn();
    inline int pickRowOffset();
    inline int pickColumnOffset();
public:
    Grid(int columns, int rows);
    ~Grid();
    
    const unsigned int ** getData();
    unsigned int getElement(int i, int j);
    void setElement(int i, int j, unsigned int value);
    
    int initializeGridRandomly(double randomRatio);
    
    void pickRandomElement(int &i, int &j);
    void pickRandomNeighbourOf(int i, int j, int &neighbourI, int &neighbourJ);
    
    static inline bool isSpeciesA(unsigned int flag)
    {
        return static_cast<bool>((flag >> SPECIES_BIT) & 1) == SPECIES_A;
    }
    
    static inline bool isSpeciesB(unsigned int flag)
    {
        return static_cast<bool>((flag >> SPECIES_BIT) & 1) == SPECIES_B;
    }
    
    inline SpeciesBit getSpecies(int column, int row)
    {
        return static_cast<SpeciesBit>((data[row][column] >> SPECIES_BIT) & 1);
    }
    
    inline void setSpecies(int column, int row, SpeciesBit species);

private:
    void allocateGrid();
    void deallocateGrid();
};


#endif //ACTIVE_MICROEMULSION_GRID_H
