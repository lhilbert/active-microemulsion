//
// Created by tommaso on 20/11/18.
//

#include "Grid.h"
#include <functional>
#include "GridInitializer.h"

int GridInitializer::initializeInnerGridAs(Grid grid, ChemicalProperties chemicalProperties, Flags flags)
{
    for (int j = grid.getFirstRow(); j <= grid.getLastRow(); ++j)
    {
        for (int i = grid.getFirstColumn(); i <= grid.getLastColumn(); ++i)
        {
            grid.setChemicalProperties(i, j, chemicalProperties);
            grid.setFlags(i, j, flags);
        }
    }
    return grid.columns * grid.rows;
}

int GridInitializer::initializeGridRandomly(Grid grid, double randomRatio, ChemicalProperties chemicalProperties,
                                            Flags flags)
{
    grid.logger.logMsg(PRODUCTION, "Initializing grid randomly: %s=%f", DUMP(randomRatio));
    
    int numRandomCells = static_cast<int>(round(grid.columns * grid.rows * randomRatio));
    int c = 0;
    int safetyCounter = 0;
    int maxIter = 5 * grid.columns * grid.rows;
    while (c < numRandomCells && safetyCounter < maxIter)
    {
        int x = 0, y = 0;
        grid.pickRandomElement(x, y);
        if (grid.chainsCellBelongsTo(x, y).empty())
        {
            grid.setChemicalProperties(x, y, chemicalProperties);
            grid.setFlags(x, y, flags);
            ++c;
        }
        ++safetyCounter;
    }
    return c;
}

std::set<ChainId> GridInitializer::initializeGridWithSingleChain(Grid grid,
                                                                 int offsetFromCenter,
                                                                 ChemicalProperties chemicalProperties,
                                                                 Flags flags,
                                                                 bool enforceChainIntegrity)
{
    grid.logger.logMsg(PRODUCTION, "Initializing grid with single horizontal chain");
    std::set<ChainId> newChains;
    
    int chainRow = (grid.rows / 2) + offsetFromCenter;
    ChainId chainId = 0;
    if (enforceChainIntegrity)
    {
        chainId = grid.getNewChainId();
        newChains.insert(chainId);
    }
    
    for (int column = 1; column <= grid.columns; ++column)
    {
        grid.setChemicalProperties(column, chainRow, chemicalProperties);
        grid.setFlags(column, chainRow, flags);
        if (enforceChainIntegrity)
        {
            grid.setChainProperties(column, chainRow, chainId,
                                     static_cast<unsigned int>(column - 1),
                                     (unsigned int) grid.rows);
        }
    }
    return newChains;
}

std::set<ChainId>
GridInitializer::initializeGridWithTwoParallelChains(Grid grid,
                                                     int distance, ChemicalProperties chemicalProperties,
                                                     Flags flags,
                                                     bool enforceChainIntegrity)
{
    grid.logger.logMsg(PRODUCTION, "Initializing grid with two parallel chains: %s=%d", DUMP(distance));
    std::set<ChainId> newChains;
    
    int chainRow = (grid.rows + distance) / 2;
    ChainId chainId1 = 0;
    ChainId chainId2 = 0;
    if (enforceChainIntegrity)
    {
        chainId1 = grid.getNewChainId();
        newChains.insert(chainId1);
        chainId2 = grid.getNewChainId();
        newChains.insert(chainId2);
    }
    
    for (int column = 1; column <= grid.columns; ++column)
    {
        grid.setChemicalProperties(column, chainRow, chemicalProperties);
        grid.setFlags(column, chainRow, flags);
        grid.setChemicalProperties(chainRow - distance, column, chemicalProperties);
        grid.setFlags(chainRow - distance, column, flags);
        if (enforceChainIntegrity)
        {
            grid.setChainProperties(column, chainRow, chainId1,
                                     static_cast<unsigned int>(column - 1),
                                     (unsigned int) grid.rows);
            grid.setChainProperties(column, chainRow - distance, chainId2,
                                     static_cast<unsigned int>(column - 1),
                                     (unsigned int) grid.rows);
        }
    }
    return newChains;
}

std::set<ChainId> GridInitializer::initializeGridWithTwoOrthogonalChains(Grid grid,
                                                                         int xOffset, int yOffset,
                                                                         ChemicalProperties chemicalProperties,
                                                                         Flags flags, bool enforceChainIntegrity)
{
    grid.logger.logMsg(PRODUCTION, "Initializing grid with two orthogonal chains: %s=%d, %s=%d", DUMP(xOffset),
                        DUMP(yOffset));
    std::set<ChainId> newChains;
    
    int chainRow = (grid.rows / 2) + yOffset;
    int chainCol = (grid.columns / 2) + xOffset;
    ChainId chainId1 = 0;
    ChainId chainId2 = 0;
    if (enforceChainIntegrity)
    {
        chainId1 = grid.getNewChainId();
        newChains.insert(chainId1);
        chainId2 = grid.getNewChainId();
        newChains.insert(chainId2);
    }
    
    for (int row = 1; row <= grid.rows; ++row)
    {
        grid.setChemicalProperties(chainCol, row, chemicalProperties);
        grid.setFlags(chainCol, row, flags);
        if (enforceChainIntegrity)
        {
            grid.setChainProperties(chainCol, row, chainId1,
                                     static_cast<unsigned int>(row - 1),
                                     (unsigned int) grid.rows);
        }
    }
    
    for (int column = 1; column <= grid.columns; ++column)
    {
        grid.setChemicalProperties(column, chainRow, chemicalProperties);
        grid.setFlags(column, chainRow, flags);
        if (enforceChainIntegrity)
        {
            grid.setChainProperties(column, chainRow, chainId2,
                                     static_cast<unsigned int>(column - 1),
                                     (unsigned int) grid.columns);
        }
    }
    
    return newChains;
}

std::set<ChainId>
GridInitializer::initializeGridWithPiShapedTwoSegmentsChain(Grid grid,
                                                            ChemicalProperties chemicalProperties, Flags flags,
                                                            bool enforceChainIntegrity)
{
    grid.logger.logMsg(PRODUCTION, "Initializing grid with \"pi\" shaped chain subdivided in 2 domains");
    std::set<ChainId> newChains;
    
    int startRow = grid.rows / 10, endRow = grid.rows + 1 - startRow;
    int startColumn = grid.columns / 3, endColumn = grid.columns + 1 - startColumn;
    int sharedColumn = startColumn + (endColumn - startColumn + 1) / 2;
    int chain1Length = (endRow - startRow) + (sharedColumn - startColumn) + 1;
    int chain2Length = (endRow - startRow) + (endColumn - sharedColumn) + 1;
    ChainId chainId1 = 0;
    ChainId chainId2 = 0;
    if (enforceChainIntegrity)
    {
        chainId1 = grid.getNewChainId();
        newChains.insert(chainId1);
        chainId2 = grid.getNewChainId();
        newChains.insert(chainId2);
    }
    
    int chain1Counter = 0, chain2Counter = 0;
    for (int row = startRow; row < endRow; ++row)
    {
        grid.setChemicalProperties(startColumn, row, chemicalProperties);
        grid.setFlags(startColumn, row, flags);
        grid.setChemicalProperties(endColumn, row, chemicalProperties);
        grid.setFlags(endColumn, row, flags);
        if (enforceChainIntegrity)
        {
            grid.setChainProperties(startColumn, row, chainId1,
                                     static_cast<unsigned int>(chain1Counter),
                                     static_cast<unsigned int>(chain1Length));
            ++chain1Counter;
            grid.setChainProperties(endColumn, row, chainId2,
                                     static_cast<unsigned int>(chain2Counter),
                                     static_cast<unsigned int>(chain2Length));
            ++chain2Counter;
        }
    }
    for (int column = startColumn; column <= sharedColumn; ++column)
    {
        grid.setChemicalProperties(column, endRow, chemicalProperties);
        grid.setFlags(column, endRow, flags);
        if (enforceChainIntegrity)
        {
            grid.setChainProperties(column, endRow, chainId1,
                                     static_cast<unsigned int>(chain1Counter),
                                     static_cast<unsigned int>(chain1Length));
            ++chain1Counter;
        }
    }
    for (int column = endColumn; column >= sharedColumn; --column)
    {
        grid.setChemicalProperties(column, endRow, chemicalProperties);
        grid.setFlags(column, endRow, flags);
        if (enforceChainIntegrity)
        {
            grid.setChainProperties(column, endRow, chainId2,
                                     static_cast<unsigned int>(chain2Counter),
                                     static_cast<unsigned int>(chain2Length));
            ++chain2Counter;
        }
    }
    if (chain1Length != chain1Counter || chain2Length != chain2Counter)
    {
        grid.logger.logMsg(ERROR,
                            "BUG! grid.initializeGridWithPiShapedTwoSegmentsChain did not compute chain length correctly! "
                            "%s=%d, %s=%d, %s=%d, %s=%d",
                            DUMP(chain1Length), DUMP(chain1Counter),
                            DUMP(chain2Length), DUMP(chain2Counter));
        grid.logger.flush();
        throw std::runtime_error("BUG! grid.initializeGridWithPiShapedTwoSegmentsChain did not compute "
                                 "chain length correctly!");
    }
    return newChains;
}

std::set<ChainId>
GridInitializer::initializeGridWithStepInstructions(Grid grid,
                                                    int &column, int &row, std::vector<Displacement> steps,
                                                    ChemicalProperties chemicalProperties, Flags flags,
                                                    bool enforceChainIntegrity)
{
    grid.logger.logMsg(PRODUCTION, "Initializing chain with step-by-step instructions");
    std::set<ChainId> newChains;
    
    ChainId chainId = 0;
    if (enforceChainIntegrity)
    {
        chainId = grid.getNewChainId();
        newChains.insert(chainId);
    }
    
    unsigned int chainLength = static_cast<unsigned int>(steps.size() + 1);
    unsigned int position = 0;
    grid.logger.logMsg(COARSE_DEBUG, "grid.initializeGridWithStepInstructions Setting cell at position (%3d,%3d)",
                        column,
                        row);
    if (!grid.isCellWithinInternalDomain(column, row))
    {
        grid.logger.logMsg(ERROR,
                            "FATAL: Trying to chain-configure cell not within internal domain! Please check your chain configuration file!");
        throw std::out_of_range("Chain configuration must take place only within internal domain!");
    }
    grid.initializeCellProperties(column, row, chemicalProperties, flags, enforceChainIntegrity,
                                   chainId, chainLength, position);
    for (Displacement step : steps)
    {
        ++position;
        grid.walkOnGrid(column, row, step.x, step.y);
        grid.logger.logMsg(COARSE_DEBUG, "grid.initializeGridWithStepInstructions Setting cell at position (%3d,%3d)",
                            column, row);
        if (!grid.isCellWithinInternalDomain(column, row))
        {
            grid.logger.logMsg(ERROR,
                                "FATAL: Trying to chain-configure cell not within internal domain! Please check your chain configuration file!");
            throw std::out_of_range("Chain configuration must take place only within internal domain!");
        }
        grid.initializeCellProperties(column, row, chemicalProperties, flags, enforceChainIntegrity,
                                       chainId, chainLength, position);
    }
    
    return newChains;
}

std::set<ChainId> GridInitializer::initializeGridWithSingleChain(Grid grid,
                                                                 std::set<ChainId> &chainSet, int offsetFromCenter,
                                                                 ChemicalProperties chemicalProperties, Flags flags,
                                                                 bool enforceChainIntegrity)
{
    std::set<ChainId> tmpSet = initializeGridWithSingleChain(grid, offsetFromCenter, chemicalProperties, flags,
                                                             enforceChainIntegrity);
    chainSet.insert(tmpSet.begin(), tmpSet.end());
    return tmpSet;
}

std::set<ChainId> GridInitializer::initializeGridWithTwoParallelChains(Grid grid,
                                                                       std::set<ChainId> &chainSet, int distance,
                                                                       ChemicalProperties chemicalProperties,
                                                                       Flags flags,
                                                                       bool enforceChainIntegrity)
{
    std::set<ChainId> tmpSet = initializeGridWithTwoParallelChains(grid, distance, chemicalProperties, flags,
                                                                   enforceChainIntegrity);
    chainSet.insert(tmpSet.begin(), tmpSet.end());
    return tmpSet;
}

std::set<ChainId>
GridInitializer::initializeGridWithTwoOrthogonalChains(Grid grid,
                                                       std::set<ChainId> &chainSet, int xOffset, int yOffset,
                                                       ChemicalProperties chemicalProperties, Flags flags,
                                                       bool enforceChainIntegrity)
{
    std::set<ChainId> tmpSet = initializeGridWithTwoOrthogonalChains(grid, xOffset, yOffset, chemicalProperties, flags,
                                                                     enforceChainIntegrity);
    chainSet.insert(tmpSet.begin(), tmpSet.end());
    return tmpSet;
}

std::set<ChainId>
GridInitializer::initializeGridWithPiShapedTwoSegmentsChain(Grid grid,
                                                            std::set<ChainId> &chainSet,
                                                            ChemicalProperties chemicalProperties,
                                                            Flags flags, bool enforceChainIntegrity)
{
    std::set<ChainId> tmpSet = initializeGridWithPiShapedTwoSegmentsChain(grid, chemicalProperties, flags,
                                                                          enforceChainIntegrity);
    chainSet.insert(tmpSet.begin(), tmpSet.end());
    return tmpSet;
}

std::set<ChainId>
GridInitializer::initializeGridWithStepInstructions(Grid grid,
                                                    std::set<ChainId> &chainSet, int &column, int &row,
                                                    std::vector<Displacement> steps,
                                                    ChemicalProperties chemicalProperties, Flags flags,
                                                    bool enforceChainIntegrity)
{
    std::set<ChainId> tmpSet = initializeGridWithStepInstructions(grid, column, row, move(steps), chemicalProperties,
                                                                  flags, enforceChainIntegrity);
    chainSet.insert(tmpSet.begin(), tmpSet.end());
    return tmpSet;
}
