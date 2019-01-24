#!/usr/bin/env python3

# chainConfigurator.py
# Script for generating chain configuration files to be used in active-microemulsion
#
# NOTE: This is python3 code but it is fully backward compatible with python2, just call the script via the interpreter.
#

import argparse
import numpy as np
from math import sqrt, floor
import random


def randomChoiceWithProbability(probability):
    return random.random() < probability


num2Direction = {0: '-', 1: '+'}
direction2Num = {v: k for k, v in num2Direction.items()}


class Step:
    def __init__(self, multiplier, displacement):
        self.multiplier = multiplier
        self.displacement = displacement

    def __repr__(self):
        return "%d%s" % (self.multiplier,
                         self.displacement.__repr__().replace(" ", "").replace("'", ""))

    @staticmethod
    def horizontalStep(multiplier=1, stepsize='+'):
        return Step(multiplier, (stepsize, 0))

    @staticmethod
    def verticalStep(multiplier=1, stepsize='+'):
        return Step(multiplier, (0, stepsize))


class Chromosome:
    species = "CHROMATIN"

    def __init__(self, boxBaseCoordinates, boxDimensions, occupancy, rotation, startCorner, active=0, transcribable=0,
                 inhibited=0, cutoff=1, sparseMode=False):
        self.active = active
        self.transcribable = transcribable
        self.inhibited = inhibited
        self.cutoff = cutoff
        self.boxBaseCoordinates = boxBaseCoordinates
        self.boxDimensions = boxDimensions
        self.startPositionOffset = [0, 0]  # The offset of the start corner relative to the box base coordinates
        self.startPosition = self.boxBaseCoordinates
        self.occupancy = occupancy
        self.sparseMode = sparseMode # Sparse mode is when we leave some space within a chromatin region
        self.rotation = rotation
        self.startCorner = startCorner
        self.initialDirections = ['+', '+']
        self.steps = []
        self.__computeInitialDirectionsAndPositionOffset(self.startCorner)
        self.__computeStartPosition()
        self.__computeChromosomeLength()
        self.__buildSteps()

    def __computeInitialDirectionsAndPositionOffset(self, startCorner):
        # Vertical direction
        if startCorner[0] == 'b':
            self.initialDirections[1] = '+'
        elif startCorner[0] == 't':
            self.initialDirections[1] = '-'
            self.startPositionOffset[1] = self.boxDimensions[1] - 1
        # Horizontal direction
        if startCorner[1] == 'l':
            self.initialDirections[0] = '+'
        elif startCorner[1] == 'r':
            self.initialDirections[0] = '-'
            self.startPositionOffset[0] = self.boxDimensions[0] - 1

    def __computeStartPosition(self):
        self.startPosition = tuple(
            [
                self.boxBaseCoordinates[i] + self.startPositionOffset[i]
                for i in range(len(self.boxBaseCoordinates))
            ]
        )

    def __computeChromosomeLength(self):
        boxArea = np.prod(self.boxDimensions)
        self.length = round(boxArea * self.occupancy)

    def __buildSteps(self):
        numRows = self.boxDimensions[1]
        numCols = self.boxDimensions[0]
        if self.rotation == 'h':
            directionLimit = numRows
            otherDirectionLimit = numCols
            direction = self.initialDirections[0]
            otherDirection = self.initialDirections[1]
            directionStep = Step.horizontalStep
            otherDirectionStep = Step.verticalStep
            self.__buildStepsInDirection(direction, otherDirection, directionStep, otherDirectionStep, directionLimit,
                                         otherDirectionLimit)
        elif self.rotation == 'v':
            directionLimit = numCols
            otherDirectionLimit = numRows
            direction = self.initialDirections[1]
            otherDirection = self.initialDirections[0]
            directionStep = Step.verticalStep
            otherDirectionStep = Step.horizontalStep
            self.__buildStepsInDirection(direction, otherDirection, directionStep, otherDirectionStep, directionLimit,
                                         otherDirectionLimit)

    def __buildStepsInDirection(self, direction, otherDirection, directionStep, otherDirectionStep, directionLimit,
                                otherDirectionLimit):
        counter = 1
        lineCounter = 1
        directionNum = direction2Num[direction]
        # Define the stride in the direction (depends on sparseMode)
        strideTheoretical = 1
        if (self.sparseMode):
            strideTheoretical = 1.0/self.occupancy # Be careful, float stride now
        strideAccumulator = 0.0

        while counter < self.length and lineCounter < directionLimit:
            stepsToGo = self.length - counter
            # Steps in the otherDirection (long ones)
            if stepsToGo < (otherDirectionLimit - 1):
                self.steps.append(directionStep(stepsToGo, num2Direction[directionNum]))
                counter += stepsToGo
            else:
                self.steps.append(directionStep(otherDirectionLimit - 1, num2Direction[directionNum]))
                counter += otherDirectionLimit - 1
            # Steps in the direction (short ones)
            #  Here compute the actual stride and keep track of any remaining part
            stride = int(floor(strideTheoretical))
            strideAccumulator += strideTheoretical - stride
            if (floor(strideAccumulator) > 0):
                stride += int(floor(strideAccumulator))
                strideAccumulator -= floor(strideAccumulator)
            #  Now make sure not to make too many steps
            stepsToGo = self.length - counter
            if stepsToGo <= 0: # Make sure not to write a 0 step
                break
            stride = min(stride, stepsToGo)
            #  Now actually perform the steps in direction
            self.steps.append(otherDirectionStep(multiplier=stride, stepsize=otherDirection))
            counter += stride
            lineCounter += stride
            directionNum = (directionNum + 1) & 1  # Here we %2 so we always have 0 or 1
        # self.steps.pop()  # Removing the last orthogonal step to stay within bounds

    def __repr__(self):
        return "%s,Active=%d,Transcribable=%d,Inhibited=%d,Cutoff=%d" % (self.species,
                                                                         self.active,
                                                                         self.transcribable,
                                                                         self.inhibited,
                                                                         self.cutoff) \
               + " : %s : " % (self.startPosition.__repr__().replace(" ", "")) \
               + " ".join([str(s) for s in self.steps])


class Configurator:

    def __init__(self, domainDimensions, numChromosomes, occupancy, inhibitionProbability, numActiveChains=-1,
                 sparseMode=False):
        self.domainDimensions = domainDimensions
        self.numChromosomes = numChromosomes
        self.occupancy = occupancy
        self.inhibitionProb = inhibitionProbability
        self.numActiveChains = numActiveChains
        self.sparseMode = sparseMode
        self.chromosomeStartPositions = []
        self.chromosomeInhibitionFlags = []
        self.__computeInhibitionFlags()
        self.chromosomes = []
        self.__generateChromosomes()

    def __computeInhibitionFlags(self):
        # TODO: here give the possibility to set just 1 active chain in the exact middle (it will be in the center of
        #  the domain). It should be driven by a specific cmdline flag.
        if self.numActiveChains < 0:
            # In this case just draw them randomly with probability
            self.chromosomeInhibitionFlags = \
                [int(randomChoiceWithProbability(self.inhibitionProb)) for x in range(self.numChromosomes)]
        else:
            numInhibitedChains = self.numChromosomes - self.numActiveChains
            self.chromosomeInhibitionFlags = [1] * numInhibitedChains + [0] * self.numActiveChains
            random.shuffle(self.chromosomeInhibitionFlags)

    def __generateChromosomes(self):
        n = round(sqrt(self.numChromosomes))
        domainWidth = self.domainDimensions[0]
        domainHeight = self.domainDimensions[1]
        hStep = int(floor(domainWidth / n))
        vStep = int(floor(domainHeight / n))
        self.chromosomeStartPositions = \
            [(x, y) for y in range(1, domainHeight + 1, vStep) for x in range(1, domainWidth + 1, hStep)]
        self.chromosomes = \
            [Chromosome(coord,
                        (hStep, vStep),
                        self.occupancy,
                        random.choice(['h', 'v']),
                        (random.choice(['b', 't']), random.choice(['l', 'r'])),
                        0,
                        0,
                        inhibitFlag,
                        int(not inhibitFlag),
                        sparseMode=self.sparseMode)
             for coord, inhibitFlag in zip(self.chromosomeStartPositions, self.chromosomeInhibitionFlags)]

    def __repr__(self):
        return "Configurator(%r,%r,%r,%r)" % (self.domainDimensions,
                                              self.numChromosomes,
                                              self.occupancy,
                                              self.inhibitionProb)

    def __str__(self):
        return "\n".join([str(chromosome) for chromosome in self.chromosomes])


if __name__ == "__main__":
    # Manage command line arguments
    parser = argparse.ArgumentParser()

    parser.add_argument("outputFile", help="The file the generated config should be written to")
    parser.add_argument("-W", "--width", help="The width of the domain (number of columns)", type=int, default=50)
    parser.add_argument("-H", "--height", help="The height of the domain (number of rows)", type=int, default=50)
    parser.add_argument("-C", "--chromatin-ratio", help="The ratio of the domain occupied by chromatin",
                        dest="chromatinRatio", type=float, default=0.5)
    parser.add_argument("-I", "--inhibition-probability",
                        help="The probability of chromatin chains to be set as inhibited",
                        dest="inhibitionProbability", type=float, default=0.5)
    parser.add_argument("-1", "--one-active-chain", help="Set just one chain as non-inhibited (overrides the -I flag)",
                        dest="oneActiveChain", action="store_true")
    parser.add_argument("--one-inhibited-chain", help="Set just one chain as inhibited (overrides the -I flag)",
                        dest="oneInhibitedChain",action="store_true")
    parser.add_argument("-A", "--number-of-active-chains", help="The number of chromatin chains to be set as active ("
                                                                "overrides the -I flag)",
                        dest="numActiveChains", type=int,
                        default=-1)
    parser.add_argument("-n", "--number-of-chains", help="The number of chromatin chains to be generated",
                        dest="numChains", type=int,
                        default=25)
    parser.add_argument("-s", "--sparse", help="Initialize chromatin blocks in the sparsest way possible (more spacing)",
                        dest="sparse", action='store_true')

    args = parser.parse_args()
    #
    nn = round(sqrt(args.numChains)) ** 2
    if nn != args.numChains:
        print("> Number of chains rounded to nearest square: %d" % (args.numChains))
        args.numChains = nn
    if args.oneActiveChain:
        args.numActiveChains = 1
    elif args.oneInhibitedChain:
        args.numActiveChains = args.numChains - 1

    chainConfigurator = Configurator((args.width, args.height), args.numChains, args.chromatinRatio,
                                     args.inhibitionProbability, args.numActiveChains, sparseMode=args.sparse)
    with open(args.outputFile, 'w') as f:
        f.write(str(chainConfigurator))

# eof
