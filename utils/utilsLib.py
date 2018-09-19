import argparse
import csv
import glob

import cv2
import numpy as np
from matplotlib import pyplot as plt
from natsort import natsorted

class Plotter:
    def __init__(self, X, plotFileName="plot.svg", interactive=True, xlabel="", ylabel=""):
        self.X = X
        self.plotFileName = plotFileName
        self.interactive = interactive
        self.xlabel = xlabel
        self.ylabel = ylabel
        self.Ys = []
        self.offsets = {}
        self.dashes = {}
        self.plotHeight = 0
        self.fig, self.ax = plt.subplots()
        self.ax.spines['right'].set_visible(False)
        self.ax.spines['top'].set_visible(False)
        self.ax.xaxis.set_ticks_position('bottom')
        self.ax.yaxis.set_ticks_position('left')

    def addYSeries(self, Y, xOffset=0, dashes=[]):
        self.Ys.append(Y)
        self.plotHeight = np.max([np.max(y) for y in self.Ys]) - np.min([np.min(y) for y in self.Ys])
        self.dashes[len(self.Ys) - 1] = dashes
        if xOffset > 0:
            self.offsets[len(self.Ys) - 1] = xOffset

    def plot(self):
        # Plotting all the series
        for id, Y in enumerate(self.Ys):
            if id in self.offsets.keys():
                offset = self.offsets[id]
                self.ax.plot(self.X[offset:offset+len(Y)], Y, dashes=self.dashes[id])
            else:
                self.ax.plot(self.X[:len(Y)], Y, dashes=self.dashes[id])

        # Apply axis labels
        plt.xlabel(self.xlabel)
        plt.ylabel(self.ylabel)

        if self.interactive:  # Here switch interactive mode off for matplotlib
            plt.show()
        else:
            plt.ioff()

        self.fig.savefig(self.plotFileName)


def computeCov(data):
    mean = np.mean(data)
    return np.std(data) / mean, mean


def getEntryNearestToValue(givenList, value):
    return min(givenList, key=lambda x: abs(x - value))


class FileSequence:
    @staticmethod
    def natsort(sequence):
        return natsorted(sequence)

    @staticmethod
    def expand(pattern):
        return FileSequence.natsort(glob.glob(pattern))

    @staticmethod
    def expandSequence(inputFileSequence):
        fileSequence = []
        for f in inputFileSequence:
            if '*' in f:  # In this case f is a pattern to expand
                fileSequence.extend(FileSequence.expand(f))
            else:
                fileSequence.append(f)
        return FileSequence.natsort(fileSequence)


class CsvWriter:
    def __init__(self, keys, dataMatrix):
        self.keys = keys
        self.data = dataMatrix

    def write(self, fileName):
        with open(fileName, 'w') as csvfile:
            datawriter = csv.writer(csvfile)
            datawriter.writerow(self.keys)
            datawriter.writerows(self.data)


class CurveAnalysis:
    def __init__(self, xAxisFileSequence, yAxisFileSequence, blurRadius=3, quiet=False, xCovMode=False, yCovMode=False):
        self.xAxisFileSequence = xAxisFileSequence
        self.yAxisFileSequence = yAxisFileSequence
        self.blurRadius = blurRadius
        self.quiet = quiet
        self.xCovMode, self.yCovMode = xCovMode, yCovMode
        self.deltaT = 1
        self.skip = 0
        self.resultsKeys = ['SnapshotNumber', 'X', 'Y']
        if self.xCovMode:
            self.resultsKeys[1] = "CoV(X)"
        if self.yCovMode:
            self.resultsKeys[2] = "CoV(Y)"
        self.results = []  # List of [snapshotNumber, CoV, meanIntensity] elements
        self.resultMatrix = None  # This will contain the numpy.ndarray of the results for easy slicing
        self.resultDict = {"id": [], "X": [], "Y": []}
        self.__analyzeSequence(self.xAxisFileSequence, self.yAxisFileSequence)
        self.numSamples = len(self.results)

    def __analyzeSnapshot(self, snapshotNum, xSnapshotFile, ySnapshotFile):
        xImg = cv2.imread(xSnapshotFile)
        yImg = cv2.imread(ySnapshotFile)
        if type(xImg) == type(None):
            print("WARNING: Image %s cannot be read. Ignoring it." % (xSnapshotFile))
            return
        if type(yImg) == type(None):
            print("WARNING: Image %s cannot be read. Ignoring it." % (ySnapshotFile))
            return
        blurredXImg = cv2.GaussianBlur(xImg, (self.blurRadius, self.blurRadius), 0)
        blurredYImg = cv2.GaussianBlur(yImg, (self.blurRadius, self.blurRadius), 0)
        xCov, xMean = computeCov(blurredXImg)
        yCov, yMean = computeCov(blurredYImg)
        xValue = xMean
        yValue = yMean
        if self.xCovMode:
            xValue = xCov
        if self.yCovMode:
            yValue = yCov
        self.results.append([snapshotNum, xValue, yValue])
        if not self.quiet:
            print("> %d : xData = %f, yData = %f" % (snapshotNum, xValue, yValue))

    def __analyzeSequence(self, xFileSequence, yFileSequence):
        for id, (x, y) in enumerate(zip(xFileSequence, yFileSequence)):
            self.__analyzeSnapshot(id, x, y)
        self.resultMatrix = np.array(self.results)
        self.resultDict["id"] = list(self.resultMatrix[:, 0])
        self.resultDict["X"] = list(self.resultMatrix[:, 1])
        self.resultDict["Y"] = list(self.resultMatrix[:, 2])

    @staticmethod
    def computeMovingAverage(sequence, windowSize):
        return np.convolve(sequence, np.ones((windowSize,)) / windowSize, mode='valid')

    def setSkip(self, skip=0):
        """
        Set the number of initial entries to skip when getting the results.
        :param skip: Positive integer
        :return: None
        """
        self.skip = skip

    def setDeltaT(self, deltaT=1):
        """
        Set the time mapping corresponding to a single timestep.
        :param deltaT: Positive integer
        :return: None
        """
        self.deltaT = deltaT

    def getNumSamples(self):
        return self.numSamples

    def getSnapshotNumber(self):
        return [self.deltaT * x for x in self.resultDict["id"][self.skip:]]

    def getXData(self):
        return self.resultDict["X"][self.skip:]

    def getYData(self):
        return self.resultDict["Y"][self.skip:]


def parseArguments(scriptNickname="script"):
    # Manage command line arguments
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("inputDir", help="The input folder where to look for Trx and RNA "
                                         "snapshots (overridden by -X and -Y)", nargs='?')
    parser.add_argument("-X", "--x-input-files",
                        help="The image file to perform measurements on, transcription channel",
                        nargs='+', dest="xInputFiles")
    parser.add_argument("-Y", "--y-input-files", help="The image file to perform measurements on, RNA channel",
                        nargs='+', dest="yInputFiles")
    parser.add_argument("-b", "--blur-radius", help="Radius of gaussian blur", dest="blurRadius", type=int, default=3)
    parser.add_argument("-m", "--moving-average-window",
                        help="Length of the moving average window. NOTE: It must be an ODD number",
                        dest="movingAvgWindow", type=int, default=7)
    parser.add_argument("-t", "--time-mapping", help="Time interval between snapshots", dest="deltaT", type=float,
                        default=1)
    parser.add_argument("-c", "--cutoff-time", help="Time point at which a generic cutoff took place", dest="cutoff",
                        type=float,
                        default=-1)
    parser.add_argument("--flavopiridol", help="Time point at which Flavopiridol was applied", dest="flavopiridol",
                        type=float,
                        default=-1)
    parser.add_argument("--actinomycin-D", help="Time point at which Actinomycin D was applied", dest="actinomycinD",
                        type=float,
                        default=-1)
    parser.add_argument("--activate", help="Time point at which transcription is activated", dest="activate",
                        type=float,
                        default=-1)
    parser.add_argument("-p", "--plot", help="Name of the desired output file for the generated plot",
                        dest="plotFileName", default=("%s_plot.svg" % scriptNickname))
    parser.add_argument("-d", "--csv", help="Name of the desired output csv for the data",
                        dest="csvFileName", default=("%s_results.csv" % scriptNickname))
    parser.add_argument("-s", "--script-mode", help="Run non-interactive. For embedding into scripts.",
                        dest="scriptMode", action='store_true')
    args = parser.parse_args()
    return args
