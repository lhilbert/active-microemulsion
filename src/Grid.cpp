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
    data = new unsigned int *[extendedRows];
    data[0] = new unsigned int[extendedRows * extendedColumns](); // "()" at the end ensure initialization to 0
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

Grid::Grid(int columns, int rows) : columns(columns),
                                    rows(rows),
                                    rowDistribution(1,rows),
                                    columnDistribution(1,columns),
                                    rowOffsetDistribution(-1,1),
                                    columnOffsetDistribution(-1,1)
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

const unsigned int **Grid::getData()
{
    return const_cast<const unsigned int **>(data);
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
            (neighbourI==i && neighbourJ==j) // We want a neighbour, not the cell itself
            || neighbourI<1 || neighbourI>columns // We want to be inside the grid
            || neighbourJ<1 || neighbourJ>rows // We want to be inside the grid
            );
}

int Grid::initializeGridRandomly(double randomRatio)
{
    int numRandomCells = static_cast<int>(round(columns * rows * randomRatio));
    for (int i=0; i<numRandomCells; ++i)
    {
        int x=0, y=0;
        pickRandomElement(x, y);
        setSpecies(x, y, SPECIES_B);
    }
    return numRandomCells;
}

void Grid::setSpecies(int column, int row, SpeciesBit species)
{
    // Set the SPECIES_BIT to the species value
    data[row][column] ^= (-species ^ data[row][column]) & (1U << SPECIES_BIT);
}

unsigned int Grid::getElement(int i, int j)
{
    return data[j][i];
}

void Grid::setElement(int i, int j, unsigned int value)
{
    data[j][i] = value;
}
