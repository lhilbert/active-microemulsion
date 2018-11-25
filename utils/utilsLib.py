import argparse
import csv
import glob

import cv2
import numpy as np
from mpl_toolkits import mplot3d
from matplotlib import pyplot as plt
from natsort import natsorted
import functools


### Functions
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
    parser.add_argument("-o", "--out-dir",
                        help="Path to a specific folder where to put the plot and data files with default names.",
                        dest="outDir", default=".")
    parser.add_argument("-p", "--plot", help="Name of the desired output file for the generated plot",
                        dest="plotFileName", default="%s_plot.svg" % (scriptNickname))
    parser.add_argument("-d", "--csv", help="Name of the desired output csv for the data",
                        dest="csvFileName", default="%s_results.csv" % (scriptNickname))
    parser.add_argument("-s", "--script-mode", help="Run non-interactive. For embedding into scripts.",
                        dest="scriptMode", action='store_true')
    parser.add_argument("-S", "--scatter-plot", help="Run in scatter-plot mode, representing data from several "
                                                     "simulations. Pass folders to scan as parameter for this option.",
                        dest="scatterPlot", nargs='+')
    parser.add_argument("-C", "--csv-input", help="CSV file to read scatterplot data from."
                                                  "This must go together with -S.",
                        dest="csvInput", action='store_true', default=False)
    parser.add_argument("-r", "--rescaling-references", help="Values to be used to rescale the axes.",
                        dest="rescalingReferences", type=float, nargs='*')
    parser.add_argument("-P", "--percentile", help="References are expressed as percentiles (0-100).",
                        dest="percentile", action='store_true', default=False)
    parser.add_argument("--thin", help="Scatterplot with thinner and less opaque dots.",
                        dest="thin", action='store_true', default=False)
    args = parser.parse_args()
    return args


def computeCov(data):
    mean = np.mean(data)
    return np.std(data) / mean, mean


def getEntryNearestToValue(givenList, value):
    return min(givenList, key=lambda x: abs(x - value))


### Classes
class Plotter:
    def __init__(self, X, plotFileName="plot.svg", interactive=True, xlabel="", ylabel="", y2label="", xlim=None,
                 ylim=None,
                 scatterPlotMode=False):
        self.X = X
        self.plotFileName = plotFileName
        self.interactive = interactive
        self.xlabel = xlabel
        self.ylabel = ylabel
        self.y2label = y2label
        self.xlim = xlim
        self.ylim = ylim
        self.scatterPlotMode = scatterPlotMode
        self.Ys = []
        self.offsets = {}
        self.dashes = {}
        self.fmt = {}
        self.axes = {}
        self.plotHeights = {}
        self.plotHeight = 0
        self.fig, self.ax = plt.subplots()
        self.ax.spines['right'].set_visible(False)
        self.ax.spines['top'].set_visible(False)
        self.ax.xaxis.set_ticks_position('bottom')
        self.ax.yaxis.set_ticks_position('left')
        self.ax2 = None
        self.plotFunctions = [None, None]

    def setAxisLim(self, xlim="unchanged", ylim="unchanged"):
        if xlim != "unchanged":
            self.xlim = xlim
        if ylim != "unchanged":
            self.ylim = ylim

    def addYSeries(self, Y, xOffset=0, dashes=None, fmt='', ax2=False):
        curAx = self.ax
        curAxId = 1
        if ax2:
            if not self.ax2:
                self.ax2 = self.ax.twinx()
                # self.ax2.spines['right'].set_visible(False)
                # self.ax2.spines['top'].set_visible(False)
                # self.ax2.xaxis.set_ticks_position('bottom')
                self.ax2.yaxis.set_ticks_position('right')
            curAx = self.ax2
            curAxId = 2

        self.Ys.append(Y)
        self.plotHeights[curAxId] = np.max([np.max(y) for y in self.Ys]) - np.min([np.min(y) for y in self.Ys])
        self.plotHeight = self.plotHeights[1]
        id = len(self.Ys) - 1
        if dashes:
            self.dashes[id] = dashes
        else:
            self.dashes[id] = []
        self.fmt[id] = fmt
        self.axes[id] = curAx
        if xOffset > 0:
            self.offsets[id] = xOffset

    def plot(self):
        # Setting the axis limits, if necessary
        self.setAxisLimits()
        # Choosing the plotting function depending on the mode
        self.configPlotFunctions()
        # Plotting all the series
        for id, Y in enumerate(self.Ys):
            # Getting the ax to use
            ax = self.axes[id]
            plotFun = self.getPlotFunction(ax)
            if id in self.offsets.keys():
                offset = self.offsets[id]
                if self.scatterPlotMode:
                    plotFun(self.X[offset:offset + len(Y)], Y, dashes=self.dashes[id])
                else:
                    plotFun(self.X[offset:offset + len(Y)], Y, self.fmt[id], dashes=self.dashes[id])
            else:
                if self.scatterPlotMode:
                    plotFun(self.X[:len(Y)], Y, dashes=self.dashes[id])
                else:
                    plotFun(self.X[:len(Y)], Y, self.fmt[id], dashes=self.dashes[id])

        # Apply axis labels
        self.ax.set_xlabel(self.xlabel)
        self.ax.set_ylabel(self.ylabel)
        if self.ax2 and self.y2label:
            self.ax2.set_ylabel(self.y2label)

        if self.interactive:  # Here switch interactive mode off for matplotlib
            plt.show()
        else:
            plt.ioff()

        self.fig.savefig(self.plotFileName)

    def setAxisLimits(self):
        # Setting the axis limits, if necessary
        if self.xlim:
            rx = self.xlim[0]
            lx = self.xlim[1]
            if rx:
                self.ax.set_xlim(right=rx)
            if lx:
                self.ax.set_xlim(right=lx)
        if self.ylim:
            bx = self.ylim[0]
            tx = self.ylim[1]
            if bx:
                self.ax.set_ylim(bottom=bx)
            if tx:
                self.ax.set_ylim(top=tx)

    def configPlotFunctions(self):
        # Choosing the plotting function depending on the mode
        if self.scatterPlotMode:
            self.plotFunctions[0] = functools.partial(self.ax.scatter, s=4)
            if self.ax2:
                self.plotFunctions[1] = functools.partial(self.ax2.scatter, s=4)
        else:
            self.plotFunctions[0] = self.ax.plot
            if self.ax2:
                self.plotFunctions[1] = self.ax2.plot

    def getPlotFunction(self, ax):
        if ax == self.ax2:
            return self.plotFunctions[1]
        else:
            return self.plotFunctions[0]


class Plotter3D:
    def __init__(self, X, Y, plotFileName="plot3D.svg", interactive=True, xlabel="", ylabel="", zlabel="", xlim=None,
                 ylim=None,
                 scatterPlotMode=False, thinMode=False):
        self.X = X
        self.Y = Y
        self.plotFileName = plotFileName
        self.interactive = interactive
        self.xlabel = xlabel
        self.ylabel = ylabel
        self.zlabel = zlabel
        self.xlim = xlim
        self.ylim = ylim
        self.scatterPlotMode = scatterPlotMode
        self.thinMode = thinMode
        self.Xs = []
        self.Ys = []
        self.Zs = []
        self.offsets = {}
        self.dashes = {}
        self.fmt = {}
        self.axes = {}
        self.plotHeights = {}
        self.plotHeight = 0
        self.fig, self.ax = plt.subplots(subplot_kw=dict(projection='3d'))
        self.ax.spines['right'].set_visible(False)
        self.ax.spines['top'].set_visible(False)
        self.ax.xaxis.set_ticks_position('bottom')
        self.ax.yaxis.set_ticks_position('bottom')
        self.ax2 = None
        self.plotFunctions = [None, None]

    def setAxisLim(self, xlim="unchanged", ylim="unchanged"):
        if xlim != "unchanged":
            self.xlim = xlim
        if ylim != "unchanged":
            self.ylim = ylim

    def addZSeries(self, Z, X=None, Y=None, xOffset=0, yOffset=0, dashes=None, fmt='', ax2=False):
        curAx = self.ax
        curAxId = 1
        if ax2:
            if not self.ax2:
                self.ax2 = self.ax.twinx()
                # self.ax2.spines['right'].set_visible(False)
                # self.ax2.spines['top'].set_visible(False)
                # self.ax2.xaxis.set_ticks_position('bottom')
                self.ax2.yaxis.set_ticks_position('right')
            curAx = self.ax2
            curAxId = 2

        if type(X) != type(None):
            self.Xs.append(X)
        else:
            self.Xs.append(self.X)
        if type(Y) != type(None):
            self.Ys.append(Y)
        else:
            self.Ys.append(self.Y)
        self.Zs.append(Z)
        self.plotHeights[curAxId] = np.max([np.max(y) for y in self.Zs]) - np.min([np.min(y) for y in self.Zs])
        self.plotHeight = self.plotHeights[1]
        id = len(self.Zs) - 1
        if dashes:
            self.dashes[id] = dashes
        else:
            self.dashes[id] = []
        self.fmt[id] = fmt
        self.axes[id] = curAx
        if xOffset > 0 or yOffset > 0:
            self.offsets[id] = (xOffset, yOffset)

    def plot(self):
        # Setting the axis limits, if necessary
        self.__setAxisLimits()
        # Choosing the plotting function depending on the mode
        self.configPlotFunctions()
        # Plotting all the series
        for id, Z in enumerate(self.Zs):
            # Getting the ax to use
            ax = self.axes[id]
            plotFun = self.getPlotFunction(ax)
            if id in self.offsets.keys():
                xOffset, yOffset = self.offsets[id]
                if self.scatterPlotMode:
                    plotFun(self.Xs[id][xOffset:xOffset + len(Z)], self.Ys[id][yOffset:yOffset + len(Z)], Z,
                            dashes=self.dashes[id])
                else:
                    plotFun(self.Xs[id][xOffset:xOffset + len(Z)], self.Ys[id][yOffset:yOffset + len(Z)], Z,
                            self.fmt[id],
                            dashes=self.dashes[id])
            else:
                if self.scatterPlotMode:
                    plotFun(self.Xs[id][:len(Z)], self.Ys[id][:len(Z)], Z, dashes=self.dashes[id])
                else:
                    plotFun(self.Xs[id][:len(Z)], self.Ys[id][:len(Z)], Z, self.fmt[id], dashes=self.dashes[id])

        # Apply axis labels
        self.ax.set_xlabel(self.xlabel)
        self.ax.set_ylabel(self.ylabel)
        self.ax.set_zlabel(self.zlabel)
        if self.ax2 and self.zlabel:
            self.ax2.set_zlabel(self.zlabel)

        if self.interactive:  # Here switch interactive mode off for matplotlib
            plt.show()
        else:
            plt.ioff()

        self.fig.savefig(self.plotFileName)

    def __setAxisLimits(self):
        # Setting the axis limits, if necessary
        if self.xlim:
            rx = self.xlim[0]
            lx = self.xlim[1]
            if rx:
                self.ax.set_xlim(right=rx)
            if lx:
                self.ax.set_xlim(right=lx)
        if self.ylim:
            bx = self.ylim[0]
            tx = self.ylim[1]
            if bx:
                self.ax.set_ylim(bottom=bx)
            if tx:
                self.ax.set_ylim(top=tx)

    def configPlotFunctions(self):
        # Choosing the plotting function depending on the mode
        if self.scatterPlotMode:
            if self.thinMode:
                self.plotFunctions[0] = functools.partial(self.ax.scatter3D, s=2, alpha=0.4)
            else:
                self.plotFunctions[0] = functools.partial(self.ax.scatter3D, s=4)
            if self.ax2:
                if self.thinMode:
                    self.plotFunctions[1] = functools.partial(self.ax2.scatter3D, s=2, alpha=0.4)
                else:
                    self.plotFunctions[1] = functools.partial(self.ax2.scatter3D, s=4)
        else:
            self.plotFunctions[0] = self.ax.plot3D
            if self.ax2:
                self.plotFunctions[1] = self.ax2.plot3D

    def getPlotFunction(self, ax):
        if ax == self.ax2:
            return self.plotFunctions[1]
        else:
            return self.plotFunctions[0]


class FileSequence:
    @staticmethod
    def natsort(sequence):
        return natsorted(sequence)

    @staticmethod
    def removeItemContainingSubstring(sequence, substring):
        targets = [ x for x in sequence if len(x.split(substring))>1 ]
        for tgt in targets:
            sequence.remove(tgt)
        return sequence

    @staticmethod
    def expand(pattern):
        return FileSequence.natsort(glob.glob(pattern))

    @staticmethod
    def expandSequence(inputFileSequence):
        fileSequence = []
        removeExtra = True
        for f in inputFileSequence:
            if "EXTRA" in f:    # If EXTRA files are explicitly passed, then do not remove them from the sequence!
                removeExtra = False
            if '*' in f:  # In this case f is a pattern to expand
                fileSequence.extend(FileSequence.expand(f))
            else:
                fileSequence.append(f)
        if removeExtra:
            res = FileSequence.removeItemContainingSubstring(FileSequence.natsort(fileSequence), "EXTRA")
        else:
            res = FileSequence.natsort(fileSequence)
        return res


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


class CurveAnalysis3D:
    def __init__(self, xAxisFileSequence=None, yAxisFileSequence=None, zAxisFileSequence=None, blurRadius=3,
                 quiet=False, xCovMode=False, yCovMode=False, zCovMode=False, csvFileName=None):
        self.xAxisFileSequence = xAxisFileSequence
        self.yAxisFileSequence = yAxisFileSequence
        self.zAxisFileSequence = zAxisFileSequence
        self.blurRadius = blurRadius
        self.quiet = quiet
        self.xCovMode, self.yCovMode, self.zCovMode = xCovMode, yCovMode, zCovMode
        self.deltaT = 1
        self.skip = 0
        self.resultsKeys = ['SnapshotNumber', 'X', 'Y', 'Z']
        if self.xCovMode:
            self.resultsKeys[1] = "CoV(X)"
        if self.yCovMode:
            self.resultsKeys[2] = "CoV(Y)"
        if self.yCovMode:
            self.resultsKeys[3] = "CoV(Z)"
        self.results = []  # List of [snapshotNumber, X, Y, Z] elements
        self.resultMatrix = None  # This will contain the numpy.ndarray of the results for easy slicing
        self.resultDict = {"id": [], "X": [], "Y": [], "Z": []}
        if csvFileName:
            self.__readDataFromCsv(csvFileName)
        elif self.xAxisFileSequence and self.yAxisFileSequence and self.zAxisFileSequence:
            self.__analyzeSequence(self.xAxisFileSequence, self.yAxisFileSequence, self.zAxisFileSequence)
        else:
            raise ValueError("You have to either pass file sequences or a CSV file to parse!")
        self.numSamples = len(self.results)

    def __analyzeSnapshot(self, snapshotNum, xSnapshotFile, ySnapshotFile, zSnapshotFile):
        xImg = cv2.imread(xSnapshotFile)
        yImg = cv2.imread(ySnapshotFile)
        zImg = cv2.imread(zSnapshotFile)
        if type(xImg) == type(None):
            print("WARNING: Image %s cannot be read. Ignoring it." % (xSnapshotFile))
            return
        if type(yImg) == type(None):
            print("WARNING: Image %s cannot be read. Ignoring it." % (ySnapshotFile))
            return
        if type(zImg) == type(None):
            print("WARNING: Image %s cannot be read. Ignoring it." % (zSnapshotFile))
            return
        blurredXImg = cv2.GaussianBlur(xImg, (self.blurRadius, self.blurRadius), 0)
        blurredYImg = cv2.GaussianBlur(yImg, (self.blurRadius, self.blurRadius), 0)
        blurredZImg = cv2.GaussianBlur(zImg, (self.blurRadius, self.blurRadius), 0)
        xCov, xMean = computeCov(blurredXImg)
        yCov, yMean = computeCov(blurredYImg)
        zCov, zMean = computeCov(blurredZImg)
        xValue = xMean
        yValue = yMean
        zValue = zMean
        if self.xCovMode:
            xValue = xCov
        if self.yCovMode:
            yValue = yCov
        if self.zCovMode:
            zValue = zCov
        self.results.append([snapshotNum, xValue, yValue, zValue])
        if not self.quiet:
            print("> %d : xData = %f, yData = %f, zData = %f" % (snapshotNum, xValue, yValue, zValue))

    def __analyzeSequence(self, xFileSequence, yFileSequence, zFileSequence):
        for id, (x, y, z) in enumerate(zip(xFileSequence, yFileSequence, zFileSequence)):
            self.__analyzeSnapshot(id, x, y, z)
        self.__updateDataStructures()

    def __updateDataStructures(self):
        self.resultMatrix = np.array(self.results)
        self.resultDict["id"] = list(self.resultMatrix[:, 0])
        self.resultDict["X"] = list(self.resultMatrix[:, 1])
        self.resultDict["Y"] = list(self.resultMatrix[:, 2])
        self.resultDict["Z"] = list(self.resultMatrix[:, 3])

    def __readDataFromCsv(self, csvFileName):
        with open(csvFileName) as csvFile:
            # First check if CSV file has header
            hasHeader = csv.Sniffer().has_header(csvFile.read(1024))
            csvFile.seek(0)  # Reset cursor to beginning of file
            if hasHeader:
                csvFile.readline()  # Skip header if any
            csvReader = csv.reader(csvFile,
                                   quoting=csv.QUOTE_NONNUMERIC)  # QUOTE_NONNUMERIC ensures numbers are converted to floats
            for id, data in enumerate(csvReader):
                item = [id]
                item.extend(data)
                self.results.append(item)
        self.__updateDataStructures()

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

    def getXData(self, scaleFactor=1.0):
        return np.asarray(self.resultDict["X"][self.skip:]) / scaleFactor

    def getYData(self, scaleFactor=1.0):
        return np.asarray(self.resultDict["Y"][self.skip:]) / scaleFactor

    def getZData(self, scaleFactor=1.0):
        return np.asarray(self.resultDict["Z"][self.skip:]) / scaleFactor

    def getXPercentile(self, q):
        return np.percentile(self.resultDict["X"][self.skip:], q)

    def getYPercentile(self, q):
        return np.percentile(self.resultDict["Y"][self.skip:], q)

    def getZPercentile(self, q):
        return np.percentile(self.resultDict["Z"][self.skip:], q)


class TimeAnalysis:
    def __init__(self, yAxisFileSequence, blurRadius=3, quiet=False, yCovMode=False):
        self.yAxisFileSequence = yAxisFileSequence
        self.blurRadius = blurRadius
        self.quiet = quiet
        self.yCovMode = yCovMode
        self.deltaT = 1
        self.skip = 0
        self.resultsKeys = ['SnapshotNumber', 'X', 'Y']
        if self.yCovMode:
            self.resultsKeys[2] = "CoV(Y)"
        self.results = []  # List of [snapshotNumber, CoV, meanIntensity] elements
        self.resultMatrix = None  # This will contain the numpy.ndarray of the results for easy slicing
        self.resultDict = {"id": [], "X": [], "Y": []}
        self.__analyzeSequence(self.yAxisFileSequence)
        self.numSamples = len(self.results)

    def __analyzeSnapshot(self, snapshotNum, ySnapshotFile):
        yImg = cv2.imread(ySnapshotFile)
        if type(yImg) == type(None):
            print("WARNING: Image %s cannot be read. Ignoring it." % (ySnapshotFile))
            return
        blurredYImg = cv2.GaussianBlur(yImg, (self.blurRadius, self.blurRadius), 0)
        yCov, yMean = computeCov(blurredYImg)
        yValue = yMean
        if self.yCovMode:
            yValue = yCov
        self.results.append([snapshotNum, snapshotNum, yValue])
        if not self.quiet:
            print("> %d : xData = %f, yData = %f" % (snapshotNum, snapshotNum, yValue))

    def __analyzeSequence(self, yFileSequence):
        for id, y in enumerate(yFileSequence):
            self.__analyzeSnapshot(id, y)
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
        return self.getSnapshotNumber()

    def getYData(self, multiplier=1):
        return multiplier * np.asarray(self.resultDict["Y"][self.skip:])
