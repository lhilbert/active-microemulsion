//
// Created by tommaso on 08/08/18.
//

#include "Microemulsion.h"
#include <omp.h>

Microemulsion::Microemulsion(Grid &grid, double omega, Logger &logger)
        : grid(grid), logger(logger), omega(omega),
          uniformProbabilityDistribution(0.0, 1.0)
{
    deltaEmin = -10 * omega;
    randomGenerator.seed(std::random_device()());
}

bool Microemulsion::performRandomSwap()
{
    int x, y, nx, ny;
    grid.pickRandomElement(x, y);
    grid.pickRandomNeighbourOf(x, y, nx, ny);
    double preEnergy = computePartialDifferentialEnergy(x, y, nx, ny);
    double postEnergy = computeSwappedPartialDifferentialEnergy(x, y, nx, ny);
    double deltaEnergy = postEnergy - preEnergy;
    double probability = computeSwapProbability(deltaEnergy);
    logger.logMsg(DEBUG, "Microemulsion::performRandomSwap - deltaEnergy=%f, probability=%f",
                  deltaEnergy, probability);
    if (randomChoiceWithProbability(probability))
    {
        unsigned int tmp = grid.getElement(x, y);
        grid.setElement(x, y, grid.getElement(nx, ny));
        grid.setElement(nx, ny, tmp);
        return true;
    }
    else
    {
        return false;
    }
}

double Microemulsion::computePartialDifferentialEnergy(int x, int y, int nx, int ny)
{
    double energy = 0;
    int dx = nx - x, dy = ny - y;
    if (dx != 0 && dy != 0)
    {
        // In this case we are trying to swap diagonally
        // First compute the energy for the selected cell...
        energy += computePairEnergy(x, y, x - dx, y);
        energy += computePairEnergy(x, y, x - dx, ny);
        energy += computePairEnergy(x, y, x, y - dy);
        energy += computePairEnergy(x, y, nx, y - dy);
        // ...then compute it for the would-be-swapped cell
        energy += computePairEnergy(nx, ny, nx + dx, y);
        energy += computePairEnergy(nx, ny, nx + dx, ny);
        energy += computePairEnergy(nx, ny, x, ny + dy);
        energy += computePairEnergy(nx, ny, nx, ny + dy);
    }
    else if (dx != 0)
    {
        // Here we are trying to swap horizontally
        for (short j = -1; j <= 1; ++j)
        {
            energy += computePairEnergy(x, y, x - dx, y + j);
            energy += computePairEnergy(nx, ny, x + dx, y + j);
        }
    }
    else if (dy != 0)
    {
        // Here we are trying to swap vertically
        for (short i = -1; i <= 1; ++i)
        {
            energy += computePairEnergy(x, y, x + i, y - dy);
            energy += computePairEnergy(nx, ny, x + i, y + dy);
        }
    }
    return energy;
}

double Microemulsion::computeSwappedPartialDifferentialEnergy(int x, int y, int nx, int ny)
{
    double energy = 0;
    int dx = nx - x, dy = ny - y;
    if (dx != 0 && dy != 0)
    {
        // In this case we are trying to swap diagonally
        // First compute the swapped-energy for the would-be-swapped cell...
        energy += computePairEnergy(nx, ny, x - dx, y);
        energy += computePairEnergy(nx, ny, x - dx, ny);
        energy += computePairEnergy(nx, ny, x, y - dy);
        energy += computePairEnergy(nx, ny, nx, y - dy);
        // ...then compute it for the selected cell
        energy += computePairEnergy(x, y, nx + dx, y);
        energy += computePairEnergy(x, y, nx + dx, ny);
        energy += computePairEnergy(x, y, x, ny + dy);
        energy += computePairEnergy(x, y, nx, ny + dy);
    }
    else if (dx != 0)
    {
        // Here we are trying to swap horizontally
        for (short j = -1; j <= 1; ++j)
        {
            energy += computePairEnergy(nx, ny, x - dx, y + j);
            energy += computePairEnergy(x, y, x + dx, y + j);
        }
    }
    else if (dy != 0)
    {
        // Here we are trying to swap vertically
        for (short i = -1; i <= 1; ++i)
        {
            energy += computePairEnergy(nx, ny, x + i, y - dy);
            energy += computePairEnergy(x, y, x + i, y + dy);
        }
    }
    return energy;
}

unsigned int Microemulsion::performRandomSwaps(unsigned int amount)
{
    //todo with coloured grid partitioning
//    unsigned int count = 0;
//    #pragma omp parallel for schedule(guided) reduce(+:count)
//    for (int i = 0; i < amount; ++i)
//    {
//        performRandomSwap();
//    }
    return 0U;
}
