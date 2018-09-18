import csv
import glob

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
