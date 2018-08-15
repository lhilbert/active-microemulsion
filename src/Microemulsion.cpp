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
    // Get a random element and a random neighbour to attempt a swap.
    grid.pickRandomElement(x, y);
    grid.pickRandomNeighbourOf(x, y, nx, ny);
    
    //Here we check if swap allowed by chains, if not we just return.
    if (!isSwapAllowedByChainsAndMeaningful(x, y, nx, ny))
    {
        logger.logMsg(DEBUG, "Microemulsion::performRandomSwap - Swap not allowed by chains! "
                             "(x=%d, y=%d) <-> (nx=%d, ny=%d)", x, y, nx, ny);
        return false;
    }
    // If chains allow the swap, then we check the energy required for it
    // and we compute its probability
    double preEnergy = computePartialDifferentialEnergy(x, y, nx, ny);
    double postEnergy = computeSwappedPartialDifferentialEnergy(x, y, nx, ny);
    double deltaEnergy = postEnergy - preEnergy;
    double probability = computeSwapProbability(deltaEnergy);
    logger.logMsg(DEBUG, "Microemulsion::performRandomSwap - deltaEnergy=%f, probability=%f",
                  deltaEnergy, probability);
    // Then we draw a random choice with the specified probability: if success we swap.
    if (randomChoiceWithProbability(probability))
    {
        CellData tmp = grid.getElement(x, y);
        grid.setElement(x, y, grid.getElement(nx, ny));
        grid.setElement(nx, ny, tmp);
        return true;
    }
    else
    {
        return false;
    }
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

bool Microemulsion::isSwapAllowedByChainsAndMeaningful(int x, int y, int nx, int ny)
{
    // Get chains of current cell and of swap candidate
    std::vector<std::reference_wrapper<ChainProperties>> chains = grid.chainsCellBelongsTo(x, y);
    std::vector<std::reference_wrapper<ChainProperties>> nChains = grid.chainsCellBelongsTo(nx, ny);
    // If neither of the cells belong to any chain, we can stop here and allow the swap.
    // HOWEVER if the cells share the same chemical properties and they don't belong
    // to chains, they are indistinguishable, so it's useless to actually swap them. In
    // this case we reject the swap anyway! //todo: to be checked (see below)
    if (chains.empty() && nChains.empty())
    {
        // If chemically indistinguishable, swap is meaningless. So we must return false.
        // todo: Check in a rigorous way if this actually ensures a speedup in the avg case.
        return !grid.areCellsChemicallyIndistinguishable(x, y, nx, ny);
    }
    // We also exclude any swap between chain neighbours, as it would break chain ordering.
    if (grid.isCellNeighbourInAnyChain(nx, ny, chains))
    {
        return false;
    }
    
    bool isSwapAllowed = false;
    int dx = nx - x, dy = ny - y;
    
    if (dx != 0 && dy != 0)
    {
        isSwapAllowed = isDiagonalSwapAllowedByChains(x, y, nx, ny, chains, nChains, dx, dy);
    }
    else if (dx != 0)
    {
        isSwapAllowed = isHorizontalSwapAllowedByChains(x, y, nx, ny, chains, nChains, dx);
    }
    else if (dy != 0)
    {
        isSwapAllowed = isVerticalSwapAllowedByChains(x, y, nx, ny, chains, nChains, dy);
    }
    return isSwapAllowed;
}

bool Microemulsion::isDiagonalSwapAllowedByChains(int x, int y, int nx, int ny,
                                                  std::vector<std::reference_wrapper<ChainProperties>> &chains,
                                                  std::vector<std::reference_wrapper<ChainProperties>> &nChains,
                                                  int dx, int dy)
{
    bool isSwapAllowed = true;
    // In this case we are swapping diagonally, chain neighbours can only be in 2 cells!
    // Furthermore, just one cell of the swapping pair can be part of chains!
    if (!chains.empty() && !nChains.empty())
    {
        isSwapAllowed = false;
    }
    else if (!chains.empty())
    {
        isSwapAllowed = true;
        for (auto& chain : chains)
        {
            if (!isSwapAllowedByChainNeighboursInDiagonalCase(x, y, dx, dy, chain))
            {
                isSwapAllowed = false;
                break;
            }
        }
    }
    else if (!nChains.empty())
    {
        isSwapAllowed = true;
        for (auto& chain : nChains)
        {
            if (!isSwapAllowedByChainNeighboursInDiagonalCase(nx, ny, -dx, -dy, chain))
            {
                isSwapAllowed = false;
                break;
            }
        }
    }
    return isSwapAllowed;
}

bool Microemulsion::isHorizontalSwapAllowedByChains(int x, int y, int nx, int ny,
                                                    std::vector<std::reference_wrapper<ChainProperties>> &chains,
                                                    std::vector<std::reference_wrapper<ChainProperties>> &nChains,
                                                    int dx)
{
    bool isSwapAllowed;
    // In this case we are swapping horizontally
    isSwapAllowed = true;
    if (!chains.empty())
    {
        // Check if swap is ok for current cell
        for (auto& chain : chains)
        {
            if (!isSwapAllowedByChainNeighboursInHorizontalCase(x, y, dx, chain))
            {
                isSwapAllowed = false;
                break;
            }
        }
    }
    if (!nChains.empty())
    {
        // Check if swap is ok for swap candidate
        for (auto& nChain : nChains)
        {
            if (!isSwapAllowedByChainNeighboursInHorizontalCase(nx, ny, -dx, nChain))
            {
                isSwapAllowed = false;
                break;
            }
        }
    }
    return isSwapAllowed;
}

bool Microemulsion::isVerticalSwapAllowedByChains(int x, int y, int nx, int ny,
                                                  std::vector<std::reference_wrapper<ChainProperties>> &chains,
                                                  std::vector<std::reference_wrapper<ChainProperties>> &nChains,
                                                  int dy)
{
    bool isSwapAllowed;
    // In this case we are swapping vertically
    isSwapAllowed = true;
    if (!chains.empty())
    {
        // Check if swap is ok for current cell
        for (auto& chain : chains)
        {
            if (!isSwapAllowedByChainNeighboursInVerticalCase(x, y, dy, chain))
            {
                isSwapAllowed = false;
                break;
            }
        }
    }
    if (!nChains.empty())
    {
        // Check if swap is ok for swap candidate
        for (auto& chain : nChains)
        {
            if (!isSwapAllowedByChainNeighboursInVerticalCase(nx, ny, -dy, chain))
            {
                isSwapAllowed = false;
                break;
            }
        }
    }
    return isSwapAllowed;
}

bool Microemulsion::isSwapAllowedByChainNeighboursInDiagonalCase(int x, int y, int dx, int dy,
                                                                 ChainProperties &chainProperties)
{
    bool horizontalCheck = grid.isCellNeighbourInChain(x + dx, y, chainProperties);
    bool verticalCheck = grid.isCellNeighbourInChain(x, y + dy, chainProperties);
    return ( horizontalCheck && verticalCheck )
           ||
           ( grid.isLastOfChain(x,y,chainProperties)
                && ( horizontalCheck || verticalCheck )
                )
            ;
}

bool Microemulsion::isSwapAllowedByChainNeighboursInHorizontalCase(int x, int y, int dx,
                                                                   ChainProperties &chainProperties)
{
    // Here it is easier to check on the complementary of the intersection of
    // the neighbours of cell and swap candidate.
    // On this area we want no neighbour, otherwise swap is impossible.
    // Also, we don't need to care about being at last position or not!
    bool isSwapAllowed = true;
    for (signed char k = -1; k <= 1; ++k)
    {
        if (grid.isCellNeighbourInChain(x-dx, y+k, chainProperties)) // checking on x-dx ensures being on the complementary
        {
            isSwapAllowed = false;
            break;
        }
    }
    return isSwapAllowed;
}

bool Microemulsion::isSwapAllowedByChainNeighboursInVerticalCase(int x, int y, int dy,
                                                                 ChainProperties &chainProperties)
{
    // Here it is easier to check on the complementary of the intersection of
    // the neighbours of cell and swap candidate.
    // On this area we want no neighbour, otherwise swap is impossible.
    // Also, we don't need to care about being at last position or not!
    bool isSwapAllowed = true;
    for (signed char k = -1; k <= 1; ++k)
    {
        if (grid.isCellNeighbourInChain(x+k, y-dy, chainProperties))
        {
            isSwapAllowed = false;
            break;
        }
    }
    return isSwapAllowed;
}
