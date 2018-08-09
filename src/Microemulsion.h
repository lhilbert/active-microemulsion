//
// Created by tommaso on 08/08/18.
//

#ifndef ACTIVE_MICROEMULSION_MICROEMULSION_H
#define ACTIVE_MICROEMULSION_MICROEMULSION_H

#include "Grid.h"
#include "Logger.h"
#include <cmath>

class Microemulsion
{
private:
    Grid &grid;
    Logger &logger;
    double omega, deltaEmin;
    std::mt19937_64 randomGenerator;
    std::uniform_real_distribution<double> uniformProbabilityDistribution;

public:
    Microemulsion(Grid &grid, double omega, Logger &logger);
    
    // The random swap returns if it was performed
    bool performRandomSwap();
    // The following attempts the given amount of swaps and returns how many
    // actually done.
    unsigned int performRandomSwaps(unsigned int amount);

private:
    double computePartialDifferentialEnergy(int x, int y, int nx, int ny);
    
    double computeSwappedPartialDifferentialEnergy(int x, int y, int nx, int ny);
    
    inline double computePairEnergy(int x, int y, int nx, int ny)
    {
        // If the species is different, return omega
        return omega * (grid.getSpecies(x, y) != grid.getSpecies(nx, ny));
    }
    
    inline double computeSwapProbability(double deltaEnergy)
    {
        return std::exp(-deltaEnergy - deltaEmin);
    }
    
    inline bool randomChoiceWithProbability(double probability)
    {
        return (uniformProbabilityDistribution(randomGenerator) < probability);
    }
};


#endif //ACTIVE_MICROEMULSION_MICROEMULSION_H
