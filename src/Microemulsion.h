//
// Created by tommaso on 08/08/18.
//

#ifndef ACTIVE_MICROEMULSION_MICROEMULSION_H
#define ACTIVE_MICROEMULSION_MICROEMULSION_H

#include "Grid.h"
#include "Logger.h"
#include <cmath>
#include <functional>

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
        return std::exp(deltaEmin - deltaEnergy);
    }
    
    inline bool randomChoiceWithProbability(double probability)
    {
        return (uniformProbabilityDistribution(randomGenerator) < probability);
    }
    
    bool isSwapAllowedByChainsAndMeaningful(int x, int y, int nx, int ny);
    
    bool isDiagonalSwapAllowedByChains(int x, int y, int nx, int ny, std::vector<std::reference_wrapper<ChainProperties>> &chains,
                                       std::vector<std::reference_wrapper<ChainProperties>> &nChains, int dx,
                                       int dy);
    
    bool isHorizontalSwapAllowedByChains(int x, int y, int nx, int ny, std::vector<std::reference_wrapper<ChainProperties>> &chains,
                                         std::vector<std::reference_wrapper<ChainProperties>> &nChains, int dx);
    
    bool isVerticalSwapAllowedByChains(int x, int y, int nx, int ny, std::vector<std::reference_wrapper<ChainProperties>> &chains,
                                         std::vector<std::reference_wrapper<ChainProperties>> &nChains, int dy);
    
    bool isSwapAllowedByChainNeighboursInDiagonalCase(int x, int y, int dx, int dy,
                                                      ChainProperties &chainProperties);
    
    bool isSwapAllowedByChainNeighboursInHorizontalCase(int x, int y, int dx,
                                                      ChainProperties &chainProperties);
    
    bool isSwapAllowedByChainNeighboursInVerticalCase(int x, int y, int dy,
                                                      ChainProperties &chainProperties);
};


#endif //ACTIVE_MICROEMULSION_MICROEMULSION_H
