//
// Created by tommaso on 08/08/18.
//

#include <algorithm>
#include "Microemulsion.h"
//#include <omp.h>

Microemulsion::Microemulsion(Grid &grid, double omega, Logger &logger, double deltaTChem, double kOn, double kOff,
                             double kChromPlus, double kChromMinus, double kRnaPlus, double kRnaMinus,
                             double kRnaTransfer, bool isBoundarySticky)
        : grid(grid), logger(logger), omega(omega),
          uniformProbabilityDistribution(0.0, 1.0),
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
    randomGenerator.seed(std::random_device()());
}

bool Microemulsion::performRandomSwap()
{
    int x, y, nx, ny;
    // Get a random element and a random neighbour to attempt a swap.
    grid.pickRandomElement(x, y);
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

bool Microemulsion::doesPairRequireEnergyCost(int x, int y, int nx, int ny) const
{
    bool isEnergyCostRequired = false;
    CellData &cellData = grid.getElement(x, y);
    CellData &nCellData = grid.getElement(nx, ny);
    // Here logic for pairs that require an omega energy cost
    if (Grid::isChromatin(cellData))
    {
        if (Grid::isActive(cellData) || Grid::getRnaContent(cellData) > 0)
        {
            isEnergyCostRequired = Grid::isChromatin(nCellData);
        }
        else
        {
            isEnergyCostRequired = Grid::isActive(nCellData) || Grid::getRnaContent(nCellData) > 0;
        }
    }
    else if (Grid::isActiveRBP(cellData))
    {
        isEnergyCostRequired = Grid::isChromatin(cellData)
                && !(Grid::isActive(nCellData) || Grid::getRnaContent(nCellData) > 0);
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
    for (int row = grid.getFirstRow(); row <= grid.getLastRow(); ++row)
    {
        for (int column = grid.getFirstColumn(); column <= grid.getLastColumn(); ++column)
        {
            // Chemical reaction takes place on each cell of the grid.
            chemicalChangesCounter += performChemicalReactionsDecay(column, row);
        }
    }
    // Phase 2:
    // Switch chromatin activity, produce RNA, transfer RNA
    for (int row = grid.getFirstRow(); row <= grid.getLastRow(); ++row)
    {
        for (int column = grid.getFirstColumn(); column <= grid.getLastColumn(); ++column)
        {
            // Chemical reaction takes place on each cell of the grid.
            chemicalChangesCounter += performChemicalReactionsProductionTransfer(column, row);
        }
    }
    return chemicalChangesCounter;
}

bool Microemulsion::performChemicalReactionsProductionTransfer(int column, int row)
{
    bool isChemPropChanged = false;
    CellData &cellData = grid.getElement(column, row);
    // 1) Switch chromatin activity level
    if (Grid::isChromatin(cellData))
    {
        // Reaction for Chromatin
        bool isTranscribable = Grid::isTranscribable(cellData);
        isChemPropChanged = performActivitySwitchingReaction(cellData,
                                                             isTranscribable * kChromPlus, kChromMinus);
        
        //todo: Check if the transcribability reaction is ok here or should be performed in a different place
        bool isTranscriptionAllowed = !Grid::isTranscriptionInhibited(cellData);
        performTranscribabilitySwitchingReaction(cellData, isTranscriptionAllowed * kOn, kOff);
    }
    // 2) Now produce and accumulate RNA on active chromatin sites
    if (Grid::isActiveChromatin(cellData))
    {
        isChemPropChanged = isChemPropChanged
                            || performRnaAccumulationReaction(cellData, kRnaPlus);
        
    }
    // 3) Now distribute RNA to RBP sites
    if (Grid::isChromatin(cellData) && Grid::getRnaContent(cellData) > 0)
    {
        //todo: What happens to the contained RNA if the chromatin is switched to inactive?
        //todo(2): Short answer: we just keep transferring it until it eventually disappears (we could alternatively also force it out)
        performRnaTransferReaction(column, row, kRnaTransfer);
    }
//    // Here below the old code to be removed
//
//    else if (Grid::isRBP(cellData))
//    {
//        // Reaction for RBP
//        bool isInProximityOfTranscription =
//                grid.doesAnyNeighbourMatchCondition(column, row,
//                                                    [](CellData &cell) {
//                                                        return Grid::isChromatin(cell) && Grid::isActive(cell);
//                                                    }
//                );
//        //debug
//        if (isInProximityOfTranscription)
//        {
//            logger.logMsg(COARSE_DEBUG, "Microemulsion::performChemicalReactionsProductionTransfer "
//                                        "Cell %s=%d, %s=%d is in proximity of transcription",
//                          DUMP(column), DUMP(row));
//        }
//        //
//        isChemPropChanged = performActivitySwitchingReaction(cellData,
//                                                             isInProximityOfTranscription * kRnaPlus, kRnaMinusRbp);
//    }
//

    return isChemPropChanged;
}

bool Microemulsion::performChemicalReactionsDecay(int column, int row)
{
    bool isChemPropChanged = false;
    CellData &cellData = grid.getElement(column, row);
    
    // 4) Now let RNA decay from active chromatin and RBP sites
    if (Grid::isRBP(cellData) || Grid::isActiveChromatin(cellData))
    {
        isChemPropChanged = isChemPropChanged
                || performRnaDecayReaction(cellData, kRnaMinusRbp);
    }
    // 5) Now set as non-active RBP sites which have reached 0 RNA
    if (Grid::isActiveRBP(cellData) && Grid::getRnaContent(cellData) == 0)
    {
        Grid::setActivity(cellData.chemicalProperties, NOT_ACTIVE);
    }
    return isChemPropChanged;
}

bool
Microemulsion::performActivitySwitchingReaction(CellData &cellData, double reactionRatePlus, double reactionRateMinus)
{
    bool isSwitched = false;
    if (Grid::isActive(cellData))
    {
        if (randomChoiceWithProbability(dtChem * reactionRateMinus))
        {
            Grid::setActivity(cellData.chemicalProperties, NOT_ACTIVE);
            isSwitched = true;
        }
    }
    else
    {
        if (randomChoiceWithProbability(dtChem * reactionRatePlus))
        {
            Grid::setActivity(cellData.chemicalProperties, ACTIVE);
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
        Grid::incrementRnaContent(cellData);
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
            Grid::decrementRnaContent(cellData);
            isSwitched = true;
        }
    }
    return isSwitched;
}

RnaCounter Microemulsion::performRnaTransferReaction(int column, int row, double transferRate)
{
    RnaCounter transferredRnaCount = 0;
    CellData &cellData = grid.getElement(column, row);
    RnaCounter rnaContent = Grid::getRnaContent(cellData);
    std::vector<std::reference_wrapper<CellData>> rbpNeighbours = grid.getNeighboursMatchingConditions(column, row,
                                                                                                       Grid::isRBP);
    
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
                Grid::decrementRnaContent(cellData);
                Grid::incrementRnaContent(randomNeighbour);
                //todo: evaluate if setting activity of RBP is now superfluous
                Grid::setActivity(randomNeighbour.chemicalProperties, ACTIVE);
            }
        }
    }
    
    return transferredRnaCount;
}

bool Microemulsion::performTranscribabilitySwitchingReaction(CellData &cellData, double reactionRatePlus,
                                                             double reactionRateMinus)
{
    bool isSwitched = false;
    if (Grid::isTranscribable(cellData))
    {
        if (randomChoiceWithProbability(dtChem * reactionRateMinus))
        {
            Grid::setTranscribability(cellData.flags, NOT_TRANSCRIBABLE);
            isSwitched = true;
        }
    }
    else
    {
        if (randomChoiceWithProbability(dtChem * reactionRatePlus))
        {
            Grid::setTranscribability(cellData.flags, TRANSCRIBABLE);
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
            std::set<ChainId> curCellChains = grid.chainsCellBelongsTo(curCell);
            std::set<ChainId> matches;
            set_intersection(curCellChains.begin(), curCellChains.end(),
                             targetChains.begin(), targetChains.end(),
                             inserter(matches, matches.begin()));
            if (!matches.empty())
            {
                Grid::setTranscriptionInhibition(curCell.flags, inhibition);
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

