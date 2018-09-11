#!/usr/bin/env python3

# chainConfigurator.py
# Script for generating chain configuration files to be used in active-microemulsion
#

import argparse
import numpy as np
from math import sqrt, floor
import random


def randomChoiceWithProbability(probability):
    return random.random() < probability


direction = {0: '-', 1: '+'}


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

    def __init__(self, boxBaseCoordinates, boxDimensions,
                 occupancy, rotation,
                 active=0, transcribable=0, inhibited=0, cutoff=1):
        self.active = active
        self.transcribable = transcribable
        self.inhibited = inhibited
        self.cutoff = cutoff
        self.boxBaseCoordinates = boxBaseCoordinates
        self.boxDimensions = boxDimensions
        self.occupancy = occupancy
        self.rotation = rotation
        self.steps = []
        self.__computeChromosomeLength()
        self.__buildSteps()

    def __computeChromosomeLength(self):
        boxArea = np.prod(self.boxDimensions)
        self.length = round(boxArea * self.occupancy)

    def __buildSteps(self):
        if self.rotation == 'h':
            numRows = self.boxDimensions[1]
            numCols = self.boxDimensions[0]
            counter = 1
            rowCounter = 1
            while counter <= self.length and rowCounter <= numRows:
                stepsToGo = self.length - counter
                if stepsToGo < (numCols - 1):
                    self.steps.append(Step.horizontalStep(stepsToGo, direction[rowCounter % 2]))
                    counter += stepsToGo
                else:
                    self.steps.append(Step.horizontalStep(numCols - 1, direction[rowCounter % 2]))
                    counter += numCols - 1
                self.steps.append(Step.verticalStep())
                counter += 1
                rowCounter += 1
            self.steps.pop()  # Removing the last vertical step to stay within bounds
        elif self.rotation == 'v':
            numRows = self.boxDimensions[1]
            numCols = self.boxDimensions[0]
            counter = 1
            colCounter = 1
            while counter <= self.length and colCounter <= numCols:
                stepsToGo = self.length - counter
                if stepsToGo < (numRows - 1):
                    self.steps.append(Step.verticalStep(stepsToGo, direction[colCounter % 2]))
                    counter += stepsToGo
                else:
                    self.steps.append(Step.verticalStep(numRows - 1, direction[colCounter % 2]))
                    counter += numRows - 1
                self.steps.append(Step.horizontalStep())
                counter += 1
                colCounter += 1
            self.steps.pop()  # Removing the last vertical step to stay within bounds

    def __repr__(self):
        return "%s,Active=%d,Transcribable=%d,Inhibited=%d,Cutoff=%d" % (self.species,
                                                                         self.active,
                                                                         self.transcribable,
                                                                         self.inhibited,
                                                                         self.cutoff) \
               + " : %s : " % (self.boxBaseCoordinates.__repr__().replace(" ", "")) \
               + " ".join([str(s) for s in self.steps])


class Configurator:

    def __init__(self, domainDimensions, numChromosomes, occupancy, inhibitionProbability):
        self.domainDimensions = domainDimensions
        self.numChromosomes = numChromosomes
        self.occupancy = occupancy
        self.inhibitionProb = inhibitionProbability
        self.chromosomeStartPositions = []
        self.chromosomes = []
        self.__generateChromosomes()

    def __generateChromosomes(self):
        n = round(sqrt(self.numChromosomes))
        domainWidth = self.domainDimensions[0]
        domainHeight = self.domainDimensions[1]
        hStep = floor(domainWidth / n)
        vStep = floor(domainHeight / n)
        self.chromosomeStartPositions = \
            [(x, y) for y in range(1, domainHeight + 1, vStep) for x in range(1, domainWidth + 1, hStep)]
        self.chromosomes = \
            [Chromosome(coord,
                        (hStep, vStep),
                        self.occupancy,
                        random.choice(['h', 'v']),
                        0,
                        0,
                        int(randomChoiceWithProbability(self.inhibitionProb)),
                        1)
             for coord in self.chromosomeStartPositions]

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
    # parser.add_argument("-t", "--test", help="Test flag", action="store_true")
    parser.add_argument("-W", "--width", help="The width of the domain (number of columns)", type=int, default=50)
    parser.add_argument("-H", "--height", help="The height of the domain (number of rows)", type=int, default=50)
    parser.add_argument("-C", "--chromatin-ratio", help="The ratio of the domain occupied by chromatin",
                        dest="chromatinRatio", type=float, default=0.5)
    parser.add_argument("-I", "--inhibition-probability",
                        help="The probability of chromatin chains to be set as inhibited",
                        dest="inhibitionProbability", type=float, default=0.5)
    parser.add_argument("-n", "--number-of-chains", help="The number of chromatin chains to be generated",
                        dest="numChains", type=int,
                        default=25)

    args = parser.parse_args()
    #
    nn = round(sqrt(args.numChains)) ** 2
    if nn != args.numChains:
        print("> Number of chains rounded to nearest square: %d" % (args.numChains))
        args.numChains = nn
    chainConfigurator = Configurator((args.width, args.height), args.numChains, args.chromatinRatio,
                                     args.inhibitionProbability)
    with open(args.outputFile, 'w') as f:
        f.write(str(chainConfigurator))

# eof
