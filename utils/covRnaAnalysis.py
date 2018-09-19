#!/usr/bin/env python3

# covRnaAnalysis.py
# Script for analyzing simulated "microscopy" images generated by active-microemulsion, it plots trajectories of
# simulations in the RNA intensity vs. DNA CoV plane.
#

import numpy as np
import os

from utilsLib import Plotter, FileSequence, CsvWriter, CurveAnalysis
from utilsLib import getEntryNearestToValue, parseArguments

scriptNickname = "covRna"
args = parseArguments(scriptNickname)

# Manage the inputDir vs. -X/-Y flags
if (not args.xInputFiles) or (not args.yInputFiles):
    if not args.inputDir:
        print("ERROR: You either need to pass the inputDir or the -X & -Y flags!")
        exit(1)
    args.xInputFiles = [os.path.join(args.inputDir, "microemulsion_RNA_*")]
    args.yInputFiles = [os.path.join(args.inputDir, "microemulsion_DNA_*")]

# Extract the base directory, so that we can by default save plot and data there
inputDirectory = os.path.dirname(args.xInputFiles[0])

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
analysis = CurveAnalysis(xFileSequence, yFileSequence, yCovMode=True, blurRadius=args.blurRadius, quiet=args.scriptMode)
analysis.setSkip(1)
analysis.setDeltaT(args.deltaT)

# Write analysis data to CSV
CsvWriter(analysis.resultsKeys, analysis.results).write(args.csvFileName)

# Plotting
plotter = Plotter(analysis.getXData(), plotFileName=args.plotFileName,
                  xlabel="RNA Intensity", ylabel="CoV(DNA)",
                  interactive=(not args.scriptMode))

if args.flavopiridol:
    eventTime = args.flavopiridol
    eventIndex = analysis.getSnapshotNumber().index(getEntryNearestToValue(analysis.getSnapshotNumber(), eventTime))
    plotter.addYSeries(analysis.getYData()[:eventIndex+1])
    plotter.addYSeries(analysis.getYData()[eventIndex:], xOffset=eventIndex, dashes=[4, 1])
elif args.actinomycinD:
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
        print(">>> Debug: x=%f, y=%f" %(nearestXToEvent, nearestYToEvent))
        print(">>> Debug: xText=%f, yText=%f" %(nearestXToEvent - 0.15 * np.max(analysis.getXData()),
                                                nearestYToEvent + 0.1 * plotter.plotHeight))
        print(">>> Debug: plotter.plotHeight=%f" %(plotter.plotHeight))
        plotter.ax.annotate('%s @ x=%d' % (event.capitalize(), eventTime),
                            xy=(nearestXToEvent, nearestYToEvent),
                            xytext=(
                                nearestXToEvent - 0.12 * np.max(analysis.getXData()),
                                nearestYToEvent + 0.15 * plotter.plotHeight),
                            arrowprops=dict(facecolor='green', shrink=1))

plotter.plot()

print("Plot saved at %s" % (args.plotFileName))
print("Data saved at %s" % (args.csvFileName))
# eof
