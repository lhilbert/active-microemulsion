//
// Created by tommaso on 08/08/18.
//

#include <algorithm>
#include "Microemulsion.h"
#include "../Utils/RandomGenerator.h"

pcg32 Microemulsion::randomGenerator = RandomGenerator::getInstance().getGenerator();
pcg64 Microemulsion::randomGenerator_64 = RandomGenerator::getInstance().getGenerator64();

Microemulsion::Microemulsion(Grid &grid, double omega, Logger &logger, double deltaTChem, double kOn, double kOff,
                             double kChromPlus, double kChromMinus, double kRnaPlus, double kRnaMinus,
                             double kRnaTransfer, bool isBoundarySticky)
        : grid(grid), logger(logger), omega(omega),
          uniformProbabilityDistribution(0.0, 1.0),
          coloursDistribution(0, 24),
          dtChem(deltaTChem),
          kOn(kOn),
          kOff(kOff),
          kChromPlus(kChromPlus),
          kChromMinus(kChromMinus),
          kRnaPlus(kRnaPlus),
          kRnaMinusRbp(kRnaMinus),
          kRnaMinusTxn(kRnaMinus),
          kRnaTransfer(kRnaTransfer),
          isBoundarySticky(isBoundarySticky)
{
    deltaEmin = -10 * fabs(omega);
    #pragma omp parallel for schedule(static,1)
    for (int i=0; i < omp_get_num_threads(); ++i)
    {
        randomGenerator = RandomGenerator::getInstance().getGenerator();
        randomGenerator_64 = RandomGenerator::getInstance().getGenerator64();
    }
}

bool Microemulsion::performRandomSwap()
{
    int x, y;
    // Get a random element and a random neighbour to attempt a swap.
    grid.pickRandomElement(x, y);
    
    return performRandomSwap(x, y);
}

bool Microemulsion::performRandomSwap(int x, int y)
{
    int nx, ny;
    grid.pickRandomNeighbourOf(x, y, nx, ny);
    
    // Here we check if swap allowed by chains, if not we just return.
    if (!isSwapAllowedByChainsAndMeaningful(x, y, nx, ny))
    {
        logger.logMsg(DEBUG, "Microemulsion::performRandomSwap - Swap not allowed by chains! "
                             "(x=%d, y=%d) <-> (nx=%d, ny=%d)", x, y, nx, ny);
        return false;
    }
    
    // Here we check if we are nearby the domain boundary and if we need to "stick" to it.
    if (isBoundarySticky && isSwapBlockedByStickyBoundary(x, y, 0, 0))
    {
        //todo: should we inhibit transcription on chromatin that sticks to the boundary?
        logger.logMsg(DEBUG, "Microemulsion::performRandomSwap - Swap not allowed by sticky boundary! "
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

unsigned int Microemulsion::performRandomSwaps(unsigned int rounds)
{
    int threads = omp_get_num_threads();
    unsigned int rVecLen = rounds/5; // This 5 is floor(pow(2^64 - 1, 1/25)), how many 25's are in a long long
    unsigned int rVecLenLoc = static_cast<unsigned int>(ceil((rVecLen + 0.0) / threads));
    rVecLen = rVecLenLoc * threads; // Rounding to make life easier
    unsigned long long *rVec = new unsigned long long[rVecLen];
    
    unsigned int count = 0;
    int colour = 0;
    #pragma omp parallel
    {
//        #pragma omp for schedule(dynamic)
//        for (unsigned int i = 0; i < rVecLen; ++i)
//        {
//            rVec[i] = randomGenerator_64();
//        }
        
        #pragma omp master
        {
            for (unsigned int i = 0; i < rVecLen; ++i)
            {
                rVec[i] = 0;
            }
        }
        #pragma omp for schedule(static,1)
        for (int t=0; t < threads; ++t)
        {
            auto *rVecLoc = new unsigned long long[rVecLenLoc];
            for (unsigned int i = 0; i < rVecLenLoc; ++i)
            {
                rVecLoc[i] = randomGenerator_64();
            }
            memcpy(rVec + (t * rVecLenLoc), rVecLoc, rVecLenLoc);
            delete[] rVecLoc;
        }
        
        for (unsigned int r = 0; r < rounds; ++r)
        {
            #pragma omp master
            {
//                colour = coloursDistribution(randomGenerator);
                unsigned int i = r / 5;
                colour = static_cast<int>(rVec[i] % 25);
                rVec[i] /= 25;
            }
            #pragma omp barrier
            unsigned char rowColour = colour / colourStride;
            unsigned char columnColour = colour % colourStride;

            #pragma omp for reduction(+:count) schedule(dynamic)
            for (int row = grid.getFirstRow() + rowColour; row < grid.getLastRow(); row += colourStride)
            {
                for (int column = grid.getFirstColumn() + columnColour;
                     column < grid.getLastColumn(); column += colourStride)
                {
                    count += performRandomSwap(column, row);
                }
            }
        }
    }
    delete[] rVec;
    return count;
}

bool Microemulsion::doesPairRequireEnergyCost(int x, int y, int nx, int ny) const
{
    bool isEnergyCostRequired = false;
    CellData &cellData = grid.getElement(x, y);
    CellData &nCellData = grid.getElement(nx, ny);
    // Here logic for pairs that require an omega energy cost
    if (cellData.isChromatin())
    {
        if (cellData.isActive() || cellData.getRnaContent() > 0)
        {
            isEnergyCostRequired = nCellData.isChromatin();
        }
        else
        {
            isEnergyCostRequired = nCellData.isActive() || nCellData.getRnaContent() > 0;
        }
    }
    else if (cellData.isActiveRBP())
    {
        isEnergyCostRequired = cellData.isChromatin()
                && !(nCellData.isActive() || nCellData.getRnaContent() > 0);
    }
    return isEnergyCostRequired;
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
    
    // Check if "meaningful".
    if (chains.empty() && nChains.empty())
    {
        // If chemically indistinguishable, swap is meaningless. So we must return false.
        // todo: Check in a rigorous way if this actually ensures a speedup in the avg case.
        return !grid.areCellsIndistinguishable(x, y, nx, ny);
    }
    
    // We also exclude any swap between chain neighbours, as it would break chain ordering!
    // todo: Here check if it is better to just check if the two cells share any chain (not just being neighbours)
    if (grid.isCellNeighbourInAnyChain(nx, ny, chains))
    {
        return false;
    }
    
    // Check if allowed, i.e. if not breaking the chain.
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
        for (auto &chain : chains)
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
        for (auto &chain : nChains)
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
        for (auto &chain : chains)
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
        for (auto &nChain : nChains)
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
        for (auto &chain : chains)
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
        for (auto &chain : nChains)
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
    return (horizontalCheck && verticalCheck)
           ||
           (grid.isLastOfChain(x, y, chainProperties)
            && (horizontalCheck || verticalCheck)
           );
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
        if (grid.isCellNeighbourInChain(x - dx, y + k,
                                        chainProperties)) // checking on x-dx ensures being on the complementary
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
        if (grid.isCellNeighbourInChain(x + k, y - dy, chainProperties))
        {
            isSwapAllowed = false;
            break;
        }
    }
    return isSwapAllowed;
}

unsigned int Microemulsion::performChemicalReactions()
{
    logger.logMsg(INFO, "Performing chemical reactions");
    unsigned int chemicalChangesCounter = 0;
    // Phase 1:
    // Decay "old" RNA
//    #pragma omp parallel
//    {
//        #pragma omp for reduction(+:chemicalChangesCounter) schedule(dynamic)
        for (int row = grid.getFirstRow(); row <= grid.getLastRow(); ++row)
        {
            for (int column = grid.getFirstColumn(); column <= grid.getLastColumn(); ++column)
            {
                // Chemical reaction takes place on each cell of the grid.
                chemicalChangesCounter += performChemicalReactionsDecay(column, row);
            }
        }
//        #pragma omp barrier
        // Phase 2:
        // Switch chromatin activity, produce RNA, transfer RNA
//        #pragma omp for reduction(+:chemicalChangesCounter) schedule(dynamic)
        for (int row = grid.getFirstRow(); row <= grid.getLastRow(); ++row)
        {
            for (int column = grid.getFirstColumn(); column <= grid.getLastColumn(); ++column)
            {
                // Chemical reaction takes place on each cell of the grid.
                chemicalChangesCounter += performChemicalReactionsProductionTransfer(column, row);
            }
        }
//    }
    return chemicalChangesCounter;
}

bool Microemulsion::performChemicalReactionsProductionTransfer(int column, int row)
{
    bool isChemPropChanged = false;
    CellData &cellData = grid.getElement(column, row);
    // 1) Switch chromatin activity level
    if (cellData.isChromatin())
    {
        // Reaction for Chromatin
        bool isTranscribable = cellData.isTranscribable();
        isChemPropChanged = performActivitySwitchingReaction(cellData,
                                                             isTranscribable * kChromPlus, kChromMinus);
        
        //todo: Check if the transcribability reaction is ok here or should be performed in a different place
        bool isTranscriptionAllowed = !cellData.isTranscriptionInhibited();
        performTranscribabilitySwitchingReaction(cellData, isTranscriptionAllowed * kOn, kOff);
    }
    // 2) Now produce and accumulate RNA on active chromatin sites
    if (cellData.isActiveChromatin())
    {
        isChemPropChanged = isChemPropChanged
                            || performRnaAccumulationReaction(cellData, kRnaPlus);
        
    }
    // 3) Now distribute RNA to RBP sites
    if (cellData.isChromatin() && cellData.getRnaContent() > 0)
    {
        //todo: What happens to the contained RNA if the chromatin is switched to inactive?
        //todo(2): Short answer: we just keep transferring it until it eventually disappears (we could alternatively also force it out)
        performRnaTransferReaction(column, row, kRnaTransfer);
    }
    
    return isChemPropChanged;
}

bool Microemulsion::performChemicalReactionsDecay(int column, int row)
{
    bool isChemPropChanged = false;
    CellData &cellData = grid.getElement(column, row);
    
    // 4) Now let RNA decay from active chromatin and RBP sites
    if (cellData.isRBP() || cellData.isActiveChromatin())
    {
        isChemPropChanged = isChemPropChanged
                || performRnaDecayReaction(cellData, kRnaMinusRbp);
    }
    // 5) Now set as non-active RBP sites which have reached 0 RNA
    if (cellData.isActiveRBP() && cellData.getRnaContent() == 0)
    {
        cellData.setActivity(NOT_ACTIVE);
    }
    return isChemPropChanged;
}

bool
Microemulsion::performActivitySwitchingReaction(CellData &cellData, double reactionRatePlus, double reactionRateMinus)
{
    bool isSwitched = false;
    if (cellData.isActive())
    {
        if (randomChoiceWithProbability(dtChem * reactionRateMinus))
        {
            cellData.setActivity(NOT_ACTIVE);
            isSwitched = true;
        }
    }
    else
    {
        if (randomChoiceWithProbability(dtChem * reactionRatePlus))
        {
            cellData.setActivity(ACTIVE);
            isSwitched = true;
        }
    }
    return isSwitched;
}

bool
Microemulsion::performRnaAccumulationReaction(CellData &cellData, double reactionRatePlus)
{
    bool isSwitched = false;
    if (randomChoiceWithProbability(dtChem * reactionRatePlus))
    {
        cellData.incrementRnaContent();
        isSwitched = true;
    }
    return isSwitched;
}

bool Microemulsion::performRnaDecayReaction(CellData &cellData, double reactionRateMinus)
{
    bool isSwitched = false;
    RnaCounter initialRnaContent = cellData.rnaContent;
    for (RnaCounter rnaUnit = 0; rnaUnit < initialRnaContent; ++rnaUnit)
    {
        if (randomChoiceWithProbability(dtChem * reactionRateMinus))
        {
            cellData.decrementRnaContent();
            isSwitched = true;
        }
    }
    return isSwitched;
}

RnaCounter Microemulsion::performRnaTransferReaction(int column, int row, double transferRate)
{
    RnaCounter transferredRnaCount = 0;
    CellData &cellData = grid.getElement(column, row);
    RnaCounter rnaContent = cellData.getRnaContent();
//    std::vector<std::reference_wrapper<CellData>> rbpNeighbours = grid.getNeighboursMatchingConditions(column, row,
//                                                                                                       CellData::isRBP);
    std::vector<std::reference_wrapper<CellData>> rbpNeighbours = grid.getNeighboursMatchingConditions(column, row,
                                                                                                       [](CellData &cd) -> bool {return cd.isRBP();}
                                                                                                       );
    
    unsigned long numNeighbours = rbpNeighbours.size();
    if (numNeighbours > 0)
    {
        std::uniform_int_distribution<unsigned char> distribution(0, static_cast<unsigned char>(numNeighbours - 1));
        for (RnaCounter rnaUnit = 0; rnaUnit < rnaContent; ++rnaUnit)
        {
            if (randomChoiceWithProbability(dtChem * transferRate * numNeighbours)) // The more the neighbours, the more the chance of being transferred
            {
                // Choose a random RBP-neighbour and transfer 1 RNA to it
                CellData &randomNeighbour = rbpNeighbours[distribution(randomGenerator)];
                cellData.decrementRnaContent();
                randomNeighbour.incrementRnaContent();
                //todo: evaluate if setting activity of RBP is now superfluous
                randomNeighbour.setActivity(ACTIVE);
            }
        }
    }
    
    return transferredRnaCount;
}

bool Microemulsion::performTranscribabilitySwitchingReaction(CellData &cellData, double reactionRatePlus,
                                                             double reactionRateMinus)
{
    bool isSwitched = false;
    if (cellData.isTranscribable())
    {
        if (randomChoiceWithProbability(dtChem * reactionRateMinus))
        {
            cellData.setTranscribability(NOT_TRANSCRIBABLE);
            isSwitched = true;
        }
    }
    else
    {
        if (randomChoiceWithProbability(dtChem * reactionRatePlus))
        {
            cellData.setTranscribability(TRANSCRIBABLE);
            isSwitched = true;
        }
    }
    return isSwitched;
}

void Microemulsion::setDtChem(double dtChem)
{
    logger.logMsg(PRODUCTION, "Microemulsion::setDtChem %s=%f", DUMP(dtChem));
    Microemulsion::dtChem = dtChem;
}

void Microemulsion::setKOn(double kOn)
{
    logger.logMsg(PRODUCTION, "Microemulsion::setKOn %s=%f", DUMP(kOn));
    Microemulsion::kOn = kOn;
}

void Microemulsion::setKOff(double kOff)
{
    logger.logMsg(PRODUCTION, "Microemulsion::setKOff %s=%f", DUMP(kOff));
    Microemulsion::kOff = kOff;
}

void Microemulsion::setKChromPlus(double kChromPlus)
{
    logger.logMsg(PRODUCTION, "Microemulsion::setKChromPlus %s=%f", DUMP(kChromPlus));
    Microemulsion::kChromPlus = kChromPlus;
}

void Microemulsion::setKChromMinus(double kChromMinus)
{
    logger.logMsg(PRODUCTION, "Microemulsion::setKChromMinus %s=%f", DUMP(kChromMinus));
    Microemulsion::kChromMinus = kChromMinus;
}

void Microemulsion::setKRnaPlus(double kRnaPlus)
{
    logger.logMsg(PRODUCTION, "Microemulsion::setKRnaPlus %s=%f", DUMP(kRnaPlus));
    Microemulsion::kRnaPlus = kRnaPlus;
}

void Microemulsion::setKRnaMinusRbp(double kRnaMinusRbp)
{
    logger.logMsg(PRODUCTION, "Microemulsion::setKRnaMinusRbp %s=%f", DUMP(kRnaMinusRbp));
    Microemulsion::kRnaMinusRbp = kRnaMinusRbp;
}

void Microemulsion::setKRnaMinusTxn(double kRnaMinusTxn)
{
    logger.logMsg(PRODUCTION, "Microemulsion::setKRnaMinusTxn %s=%f", DUMP(kRnaMinusTxn));
    Microemulsion::kRnaMinusTxn = kRnaMinusTxn;
}

void Microemulsion::setKRnaTransfer(double kRnaTransfer)
{
    Microemulsion::kRnaTransfer = kRnaTransfer;
}

void Microemulsion::setTranscriptionInhibitionOnChains(const std::set<ChainId> &targetChains,
                                                       const TranscriptionInhibition &inhibition) const
{
    for (int row = grid.getFirstRow(); row <= grid.getLastRow(); ++row)
    {
        for (int column = grid.getFirstColumn(); column <= grid.getLastColumn(); ++column)
        {
            CellData &curCell = grid.getElement(column, row);
            std::set<ChainId> curCellChains = curCell.chainsCellBelongsTo();
            std::set<ChainId> matches;
            set_intersection(curCellChains.begin(), curCellChains.end(),
                             targetChains.begin(), targetChains.end(),
                             inserter(matches, matches.begin()));
            if (!matches.empty())
            {
                curCell.setTranscriptionInhibition(inhibition);
            }
        }
    }
}

void Microemulsion::enableTranscribabilityOnChains(std::set<ChainId> targetChains)
{
    logger.logMsg(PRODUCTION, "Transcription enabled on %d chains", targetChains.size());
    setTranscriptionInhibitionOnChains(targetChains, TRANSCRIPTION_POSSIBLE);
}

void Microemulsion::disableTranscribabilityOnChains(std::set<ChainId> targetChains)
{
    logger.logMsg(PRODUCTION, "Transcription inhibited on %d chains", targetChains.size());
    setTranscriptionInhibitionOnChains(targetChains, TRANSCRIPTION_INHIBITED);
}

bool Microemulsion::isSwapBlockedByStickyBoundary(int x, int y, int nx, int ny)
{
    // todo: check if this has to be applied to all chromatin or just to inactive one!
    return (grid.isPositionNextToBoundary(x, y) && grid.isInactiveChromatin(x, y))
           || (grid.isPositionNextToBoundary(nx, ny) && grid.isInactiveChromatin(nx, ny));
}

