#!/usr/bin/env python3

# covRnaAnalysis.py
# Script for analyzing simulated "microscopy" images generated by active-microemulsion, it plots trajectories of
# simulations in the RNA intensity vs. DNA CoV plane.
#

import numpy as np
import os

from utilsLib import Plotter, FileSequence, CsvWriter, TimeAnalysis
from utilsLib import getEntryNearestToValue, parseArguments

scriptNickname = "covRnaTxnTime"
args = parseArguments(scriptNickname)

# Manage the inputDir vs. -X/-Y flags
if args.xInputFiles or args.yInputFiles or args.scatterPlot:
    print("ERROR: Options -X, -Y & -S are not supported!")
    exit(1)

if not args.inputDir:
    print("ERROR: You either need to pass the input directory!")
    exit(1)
DNAInputFiles = [os.path.join(args.inputDir, "microemulsion_DNA_*")]
RNAInputFiles = [os.path.join(args.inputDir, "microemulsion_RNA_*")]
TXNInputFiles = [os.path.join(args.inputDir, "microemulsion_Transcription_*")]
# Extract the base directory, so that we can by default save plot and data there
inputDirectory = os.path.dirname(DNAInputFiles[0])

# Using input dir for plot if none was explicitly passed
plotDirectory, plotFilename = os.path.split(args.plotFileName)
if not plotDirectory:
    plotDirectory = inputDirectory
args.plotFileName = os.path.join(plotDirectory, plotFilename)

# Using input dir for csv if none was explicitly passed
csvDirectory, csvFilename = os.path.split(args.csvFileName)
if not csvDirectory:
    csvDirectory = inputDirectory
args.csvFileName = os.path.join(csvDirectory, csvFilename)

# Expanding and sorting the file list
DNAFileSequence = FileSequence(DNAInputFiles)
RNAFileSequence = FileSequence(RNAInputFiles)
TXNFileSequence = FileSequence(TXNInputFiles)

# Perform the actual analysis
covAnalysis = TimeAnalysis(DNAFileSequence, yCovMode=True, blurRadius=args.blurRadius, quiet=args.scriptMode)
covAnalysis.setSkip(1)
covAnalysis.setDeltaT(args.deltaT)

rnaAnalysis = TimeAnalysis(RNAFileSequence, yCovMode=False, blurRadius=args.blurRadius, quiet=args.scriptMode)
rnaAnalysis.setSkip(1)
rnaAnalysis.setDeltaT(args.deltaT)

txnAnalysis = TimeAnalysis(TXNFileSequence, yCovMode=False, blurRadius=args.blurRadius, quiet=args.scriptMode)
txnAnalysis.setSkip(1)
txnAnalysis.setDeltaT(args.deltaT)

# Write analysis data to CSV
keys = [covAnalysis.resultsKeys[0], covAnalysis.resultsKeys[2], rnaAnalysis.resultsKeys[2], txnAnalysis.resultsKeys[2]]
results = [
    [covAnalysis.results[i][0], covAnalysis.results[i][2], rnaAnalysis.results[i][2], txnAnalysis.results[i][2]]
    for i in range(len(covAnalysis.results))
]
CsvWriter(keys, results).write(args.csvFileName)

# Plotting
plotter = Plotter(covAnalysis.getXData(), plotFileName=args.plotFileName,
                  xlabel="Time", ylabel="CoV(DNA)", y2label="RNA/TXN Intensity",
                  interactive=(not args.scriptMode),
                  scatterPlotMode=(not not args.scatterPlot))

if args.flavopiridol > 0:
    eventTime = args.flavopiridol
    eventIndex = covAnalysis.getSnapshotNumber().index(getEntryNearestToValue(covAnalysis.getSnapshotNumber(), eventTime))
    plotter.addYSeries(covAnalysis.getYData()[:eventIndex + 1])
    plotter.addYSeries(covAnalysis.getYData()[eventIndex:], xOffset=eventIndex, dashes=[4, 1])
    plotter.addYSeries(rnaAnalysis.getYData()[:eventIndex + 1], fmt='g', ax2=True)
    plotter.addYSeries(rnaAnalysis.getYData()[eventIndex:], xOffset=eventIndex, dashes=[4, 1], fmt='g', ax2=True)
    plotter.addYSeries(txnAnalysis.getYData()[:eventIndex + 1], fmt='m', ax2=True)
    plotter.addYSeries(txnAnalysis.getYData()[eventIndex:], xOffset=eventIndex, dashes=[4, 1], fmt='m', ax2=True)
elif args.actinomycinD > 0:
    eventTime = args.actinomycinD
    eventIndex = covAnalysis.getSnapshotNumber().index(getEntryNearestToValue(covAnalysis.getSnapshotNumber(), eventTime))
    plotter.addYSeries(covAnalysis.getYData()[:eventIndex + 1])
    plotter.addYSeries(covAnalysis.getYData()[eventIndex:], xOffset=eventIndex, dashes=[4, 1])
    plotter.addYSeries(rnaAnalysis.getYData()[:eventIndex + 1], fmt='g', ax2=True)
    plotter.addYSeries(rnaAnalysis.getYData()[eventIndex:], xOffset=eventIndex, dashes=[4, 1], fmt='g', ax2=True)
    plotter.addYSeries(txnAnalysis.getYData()[:eventIndex + 1], fmt='m', ax2=True)
    plotter.addYSeries(txnAnalysis.getYData()[eventIndex:], xOffset=eventIndex, dashes=[4, 1], fmt='m', ax2=True)
else:
    plotter.addYSeries(covAnalysis.getYData())
    plotter.addYSeries(rnaAnalysis.getYData(), fmt='g', ax2=True)
    # plotter.addYSeries(np.add(10*rnaAnalysis.getYData(), txnAnalysis.getYData()), fmt='g', ax2=True)
    plotter.addYSeries(txnAnalysis.getYData(), fmt='m', ax2=True)

for event in ["flavopiridol", "actinomycinD", "cutoff"]:
    eventTime = getattr(args, event)
    if eventTime > 0:
        nearestTimeToEvent = getEntryNearestToValue(covAnalysis.getSnapshotNumber(), eventTime)
        nearestXToEvent = covAnalysis.getXData()[covAnalysis.getSnapshotNumber().index(nearestTimeToEvent)]
        nearestYToEvent = covAnalysis.getYData()[covAnalysis.getSnapshotNumber().index(nearestTimeToEvent)]
        print(">>> Debug: x=%f, y=%f" % (nearestXToEvent, nearestYToEvent))
        print(">>> Debug: xText=%f, yText=%f" % (nearestXToEvent - 0.15 * np.max(covAnalysis.getXData()),
                                                 nearestYToEvent + 0.1 * plotter.plotHeight))
        print(">>> Debug: plotter.plotHeight=%f" % (plotter.plotHeight))
        plotter.ax.annotate('%s @ t=%d' % (event.capitalize(), eventTime),
                            xy=(nearestXToEvent, nearestYToEvent),
                            xytext=(
                                nearestXToEvent - 0.12 * np.max(covAnalysis.getXData()),
                                nearestYToEvent + 0.15 * plotter.plotHeight),
                            arrowprops=dict(facecolor='green', shrink=1))

plotter.plot()

print("Plot saved at %s" % (args.plotFileName))
print("Data saved at %s" % (args.csvFileName))
# eof
