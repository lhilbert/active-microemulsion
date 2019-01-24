//
// Created by tommaso on 20/11/18.
//

#ifndef ACTIVE_MICROEMULSION_GRIDINITIALIZER_H
#define ACTIVE_MICROEMULSION_GRIDINITIALIZER_H


#include <functional>
#include <set>
#include "../Cell/CellData.h"
#include "../Chain/ChainConfig.h"
#include "Grid.h"

class GridInitializer
{

public:
    static int initializeInnerGridAs(Grid &grid, ChemicalProperties chemicalProperties, Flags flags = 0);
    
    static int initializeGridRandomly(Grid &grid, double randomRatio, ChemicalProperties chemicalProperties, Flags flags = 0);
    
    /**
         * Add a single horizontal chain of the specified characteristics to the grid.
         * @param offsetFromCenter
         * @param chemicalProperties
         * @param flags
         * @param enforceChainIntegrity
         * @return A set containing the ID assigned to the new chain.
         */
    static std::set<ChainId>
    initializeGridWithSingleChain(Grid &grid, int offsetFromCenter, ChemicalProperties chemicalProperties, Flags flags = 0,
                                  bool enforceChainIntegrity = true);
    
    /**
         * Add a two horizontal chains of the specified characteristics, and with given distance, to the grid.
         * @param distance
         * @param chemicalProperties
         * @param flags
         * @param enforceChainIntegrity
         * @return A set containing the IDs assigned to the new chains.
         */
    static std::set<ChainId> initializeGridWithTwoParallelChains(Grid &grid, int distance, ChemicalProperties chemicalProperties,
                                                          Flags flags = 0, bool enforceChainIntegrity = true);
    
    /**
         * Add a horizontal and a vertical chains of the specified characteristics, crossing at the given point, to the grid.
         * @param xOffset
         * @param yOffset
         * @param chemicalProperties
         * @param flags
         * @param enforceChainIntegrity
         * @return A set containing the IDs assigned to the new chains.
         */
    static std::set<ChainId> initializeGridWithTwoOrthogonalChains(Grid &grid, int xOffset, int yOffset,
                                                            ChemicalProperties chemicalProperties, Flags flags = 0,
                                                            bool enforceChainIntegrity = true);
    
    static std::set<ChainId> initializeGridWithPiShapedTwoSegmentsChain(Grid &grid, ChemicalProperties chemicalProperties, Flags flags = 0,
                                                                 bool enforceChainIntegrity = true);
    
    static std::set<ChainId>
    initializeGridWithStepInstructions(Grid &grid, int &column, int &row, std::vector<Displacement> steps,
                                       ChemicalProperties chemicalProperties,
                                       Flags flags = 0,
                                       bool enforceChainIntegrity = true);
    
    /**
         * Add a single horizontal chain of the specified characteristics to the grid.
         * Extend a given chain id set with the id of the new chain.
         * @param chainSet
         * @param offsetFromCenter
         * @param chemicalProperties
         * @param flags
         * @param enforceChainIntegrity
         * @return A set containing the ID assigned to the new chain.
         */
    static std::set<ChainId>
    initializeGridWithSingleChain(Grid &grid, std::set<ChainId> &chainSet,
                                  int offsetFromCenter, ChemicalProperties chemicalProperties, Flags flags = 0,
                                  bool enforceChainIntegrity = true);
    
    /**
         * Add a two horizontal chains of the specified characteristics, and with given distance, to the grid.
         * Extend a given chain id set with the id of the new chain.
         * @param chainSet
         * @param distance
         * @param chemicalProperties
         * @param flags
         * @param enforceChainIntegrity
         * @return A set containing the IDs assigned to the new chains.
         */
    static std::set<ChainId> initializeGridWithTwoParallelChains(Grid &grid, std::set<ChainId> &chainSet,
                                                          int distance, ChemicalProperties chemicalProperties,
                                                          Flags flags = 0, bool enforceChainIntegrity = true);
    
    /**
         * Add a horizontal and a vertical chains of the specified characteristics, crossing at the given point, to the grid.
         * Extend a given chain id set with the id of the new chain.
         * @param chainSet
         * @param xOffset
         * @param yOffset
         * @param chemicalProperties
         * @param flags
         * @param enforceChainIntegrity
         * @return A set containing the IDs assigned to the new chains.
         */
    static std::set<ChainId> initializeGridWithTwoOrthogonalChains(Grid &grid, std::set<ChainId> &chainSet,
                                                            int xOffset, int yOffset,
                                                            ChemicalProperties chemicalProperties, Flags flags = 0,
                                                            bool enforceChainIntegrity = true);
    
    static std::set<ChainId> initializeGridWithPiShapedTwoSegmentsChain(Grid &grid, std::set<ChainId> &chainSet,
                                                                 ChemicalProperties chemicalProperties, Flags flags = 0,
                                                                 bool enforceChainIntegrity = true);
    
    static std::set<ChainId> initializeGridWithStepInstructions(Grid &grid, std::set<ChainId> &chainSet,
                                                         int &column, int &row, std::vector<Displacement> steps,
                                                         ChemicalProperties chemicalProperties,
                                                         Flags flags = 0,
                                                         bool enforceChainIntegrity = true);
};


#endif //ACTIVE_MICROEMULSION_GRIDINITIALIZER_H
