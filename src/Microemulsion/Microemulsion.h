//
// Created by tommaso on 08/08/18.
//

#ifndef ACTIVE_MICROEMULSION_MICROEMULSION_H
#define ACTIVE_MICROEMULSION_MICROEMULSION_H

#include "../Grid/Grid.h"
#include "../Logger/Logger.h"
#include <cmath>
#include <functional>
#include <random>

class Microemulsion
{
public:
    static const int colourStride = 5;
    
private:
    Grid &grid;
    Logger &logger;
    double omega, deltaEmin;
    static std::mt19937 randomGenerator;
    #pragma omp threadprivate(randomGenerator)
    static std::mt19937_64 randomGenerator_64;
    #pragma omp threadprivate(randomGenerator_64)
    std::uniform_real_distribution<double> uniformProbabilityDistribution;
    std::uniform_int_distribution<int> coloursDistribution;
    double dtChem, kOn, kOff, kChromPlus, kChromMinus, kRnaPlus, kRnaMinusRbp, kRnaMinusTxn, kRnaTransfer;
    bool isBoundarySticky;

public:
    Microemulsion(Grid &grid, double omega, Logger &logger, double deltaTChem, double kOn, double kOff,
                      double kChromPlus, double kChromMinus, double kRnaPlus, double kRnaMinus,
                      double kRnaTransfer, bool isBoundarySticky);
    
    void setDtChem(double dtChem);
    
    void setKOn(double kOn);
    
    void setKOff(double kOff);
    
    void setKChromPlus(double kChromPlus);
    
    void setKChromMinus(double kChromMinus);
    
    void setKRnaPlus(double kRnaPlus);
    
    void setKRnaMinusRbp(double kRnaMinusRbp);

    void setKRnaMinusTxn(double kRnaMinusTxn);
    
    void setKRnaTransfer(double kRnaTransfer);
    
    /**
     * Attempts a random swap between two neighbouring cells on the grid.
     * @return True if the swap was performed.
     */
    bool performRandomSwap();
    
    /**
     * Attempts the given amount of swaps and returns how many actually done.
     * @param rounds The number of swaps to attempt.
     * @return The number of swaps successfully performed.
     */
    unsigned int performRandomSwaps(unsigned int rounds);
    
    /**
     * Perform the chemical reactions on the entire grid.
     * @return The total number of chemical changes.
     */
    unsigned int performChemicalReactions();
    
    /**
     * Switch the given chain to the transcribable state.
     * @param targetChains
     */
    void enableTranscribabilityOnChains(std::set<ChainId> targetChains);
    
    void disableTranscribabilityOnChains(std::set<ChainId> targetChains);
    
    void
    setTranscriptionInhibitionOnChains(const std::set<ChainId> &targetChains, const TranscriptionInhibition &inhibition) const;

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
        // The below is here just as a reminder for possible future tests
        // double prob = std::exp(-deltaEnergy);
        // return fmin(prob, 1);
    }
    
    inline bool randomChoiceWithProbability(double probability)
    {
        return (uniformProbabilityDistribution(randomGenerator) < probability);
    }
    
    bool isSwapBlockedByStickyBoundary(int x, int y, int nx, int ny);
    
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
    bool performChemicalReactionsProductionTransfer(int column, int row);
    
    bool performChemicalReactionsDecay(int column, int row);
    
    bool performActivitySwitchingReaction(CellData &cellData, double reactionRatePlus, double reactionRateMinus);
    
    bool performRnaAccumulationReaction(CellData &cellData, double reactionRatePlus);
    
    bool performRnaDecayReaction(CellData &cellData, double reactionRateMinus);
    
    RnaCounter performRnaTransferReaction(int column, int row, double transferRate);
    
    bool performTranscribabilitySwitchingReaction(CellData &cellData, double reactionRatePlus, double reactionRateMinus);
    
    bool performRandomSwap(int x, int y);
};


#endif //ACTIVE_MICROEMULSION_MICROEMULSION_H
