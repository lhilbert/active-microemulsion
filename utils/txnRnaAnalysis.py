#!/usr/bin/env python3

# trxRnaAnalysis.py
# Script for analyzing simulated "microscopy" images generated by active-microemulsion, it plots trajectories of
# simulations in the Trx vs. RNA intensities plane.
#

import argparse

import cv2
import numpy as np
import os

from utilsLib import Plotter, FileSequence, CsvWriter, CurveAnalysis
from utilsLib import getEntryNearestToValue, parseArguments

scriptNickname = "trxRna"
args = parseArguments(scriptNickname)

# Manage the inputDir vs. -X/-Y flags
if (not args.xInputFiles) or (not args.yInputFiles):
    if args.scatterPlot:
        args.xInputFiles = [os.path.join(d, "microemulsion_Transcription_EXTRA.pgm") for d in args.scatterPlot]
        args.yInputFiles = [os.path.join(d, "microemulsion_RNA_EXTRA.pgm") for d in args.scatterPlot]
        inputDirectory = "." # Scatterplot data goes in current dir
    else:
        if not args.inputDir:
            print("ERROR: You either need to pass the inputDir or the -X & -Y flags!")
            exit(1)
        args.xInputFiles = [os.path.join(args.inputDir, "microemulsion_Transcription_*")]
        args.yInputFiles = [os.path.join(args.inputDir, "microemulsion_RNA_*")]
        # Extract the base directory, so that we can by default save plot and data there
        inputDirectory = os.path.dirname(args.xInputFiles[0])
else:
    inputDirectory = os.path.dirname(args.trxInputFiles[0])

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
xFileSequence = FileSequence.expandSequence(args.xInputFiles)
yFileSequence = FileSequence.expandSequence(args.yInputFiles)

# Perform the actual analysis
analysis = CurveAnalysis(xFileSequence, yFileSequence, blurRadius=args.blurRadius, quiet=args.scriptMode)
if args.scatterPlot:
    analysis.setSkip(0)
else:
    analysis.setSkip(1)
analysis.setDeltaT(args.deltaT)

# Write analysis data to CSV
CsvWriter(analysis.resultsKeys, analysis.results).write(args.csvFileName)

# Plotting
plotter = Plotter(analysis.getXData(), plotFileName=args.plotFileName,
                  xlabel="Transcription Intensity", ylabel="RNA Intensity",
                  interactive=(not args.scriptMode),
                  scatterPlotMode=(not not args.scatterPlot),
                  xlim=(0,60), ylim=(0, 8))

if args.flavopiridol > 0:
    eventTime = args.flavopiridol
    eventIndex = analysis.getSnapshotNumber().index(getEntryNearestToValue(analysis.getSnapshotNumber(), eventTime))
    plotter.addYSeries(analysis.getYData()[:eventIndex+1])
    plotter.addYSeries(analysis.getYData()[eventIndex:], xOffset=eventIndex, dashes=[4, 1])
elif args.actinomycinD > 0:
    eventTime = args.actinomycinD
    eventIndex = analysis.getSnapshotNumber().index(getEntryNearestToValue(analysis.getSnapshotNumber(), eventTime))
    plotter.addYSeries(analysis.getYData()[:eventIndex+1])
    plotter.addYSeries(analysis.getYData()[eventIndex:], xOffset=eventIndex, dashes=[4, 1])
else:
    plotter.addYSeries(analysis.getYData())

for event in ["flavopiridol", "actinomycinD", "cutoff"]:
    eventTime = getattr(args, event)
    if eventTime > 0:
        nearestTimeToEvent = getEntryNearestToValue(analysis.getSnapshotNumber(), eventTime)
        nearestXToEvent = analysis.getXData()[analysis.getSnapshotNumber().index(nearestTimeToEvent)]
        nearestYToEvent = analysis.getYData()[analysis.getSnapshotNumber().index(nearestTimeToEvent)]
        plotter.ax.annotate('%s @ t=%d' % (event.capitalize(), eventTime),
                            xy=(nearestXToEvent, nearestYToEvent),
                            xytext=(
                                nearestXToEvent - 0.15 * np.max(analysis.getXData()),
                                nearestYToEvent + 0.1 * plotter.plotHeight),
                            arrowprops=dict(facecolor='green', shrink=1))

plotter.plot()

print("Plot saved at %s" % (args.plotFileName))
print("Data saved at %s" % (args.csvFileName))
# eof
