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
    double dtChem, kOn, kOff, kChromPlus, kChromMinus, kRnaPlus, kRnaMinus;

public:
    Microemulsion(Grid &grid, double omega, Logger &logger,
                  double deltaTChem,
                  double kOn,
                  double kOff,
                  double kChromPlus,
                  double kChromMinus,
                  double kRnaPlus,
                  double kRnaMinus);
    
    void setDtChem(double dtChem);
    
    void setKOn(double kOn);
    
    void setKOff(double kOff);
    
    void setKChromPlus(double kChromPlus);
    
    void setKChromMinus(double kChromMinus);
    
    void setKRnaPlus(double kRnaPlus);
    
    void setKRnaMinus(double kRnaMinus);
    
    /**
     * Attempts a random swap between two neighbouring cells on the grid.
     * @return True if the swap was performed.
     */
    bool performRandomSwap();
    
    /**
     * Attempts the given amount of swaps and returns how many actually done.
     * @param amount The number of swaps to attempt.
     * @return The number of swaps successfully performed.
     */
    unsigned int performRandomSwaps(unsigned int amount);
    
    /**
     * Perform the chemical reactions on the entire grid.
     * @return The total number of chemical changes.
     */
    unsigned int performChemicalReactions();

private:
    double computePartialDifferentialEnergy(int x, int y, int nx, int ny);
    
    double computeSwappedPartialDifferentialEnergy(int x, int y, int nx, int ny);
    
    inline double computePairEnergy(int x, int y, int nx, int ny)
    {
        // If the species is different, return omega
        return omega * doesPairRequireEnergyCost(x, y, nx, ny);
    }
    
    bool doesPairRequireEnergyCost(int x, int y, int nx, int ny) const;
    
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
    
    // If reaction causes a change in chemical properties, return True.
    bool performChemicalReaction(int row, int column);
    
    
};


#endif //ACTIVE_MICROEMULSION_MICROEMULSION_H
