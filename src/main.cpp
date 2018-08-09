#include <iostream>
#include "../lib/argparse/argparse.hpp"
#include "Logger.h"
#include "Grid.h"
#include "Microemulsion.h"
#include "PgmWriter.h"

int main(int argc, const char **argv)
{
    // Command-line argument parser. We use the argparse library (https://github.com/hbristow/argparse).
    ArgumentParser argparser;
    // Here below some arguments, to be completed.
    argparser.addArgument("--debug");
    argparser.addArgument("-o", "--output-dir", 1);
    argparser.addArgument("-i", "--input-image", 1);
    argparser.addArgument("-T", "--end-time", 1);
    
    argparser.parse(static_cast<size_t>(argc), argv);
    
//    bool debugMode = argparser.exists("debug");
    int rows = 200, columns = 200; //todo: then read these from configuration
//    int endTime = argparser.retrieve<int>("end-time");
    double endTime = 1e3;
    double dt = 1e-5; // The dt used for timestepping. //todo read this from config
    double dtOut = 10; // The dt used for visualization output. //todo read this from config
    double omega = 0.3; //todo read this from config
    
    // Setup and start Logger
    Logger logger;
//    if (debugMode)
//    {
//        logger.setDebugLevel(DEBUG);
//    }
//    logger.setDebugLevel(DEBUG);
    logger.openLogFile();
    logger.setStartTime();
    logger.logMsg(logger.getDebugLevel(), "Debug level set to %s", logger.getDebugLevelStr());
    
    // Config-file parser
    //logger.logMsg(PRODUCTION, "Reading configuration");
    //todo
    
    // Initialize data structures
    logger.logMsg(INFO, "Initializing grid");
    Grid grid(columns, rows);
    int numRandomCells = grid.initializeGridRandomly(0.3);
    logger.logMsg(INFO, "GRID: numRandomCells=%d", numRandomCells);
    logger.logMsg(INFO, "Initializing microemulsion");
    Microemulsion microemulsion(grid, omega, logger);
    
    // Initialize PgmWriter
    logger.logMsg(INFO, "Initializing PGM writer");
    PgmWriter pgmWriter(columns, rows, 1, "test.pgm");
    pgmWriter.setData(grid.getData());
    // Write initial data to file
    logger.logMsg(PRODUCTION, "Writing PGM file #%d", pgmWriter.getCounter());
    pgmWriter.write();
    pgmWriter.advanceSeries();
    
    // Simulation loops
    double t = 0;
    double lastOutputTime = 0;
    unsigned long swapAttempts = 0;
    unsigned long swapsPerformed = 0;
    while (t < endTime)
    {
        // Time-stepping loop, do something here
        swapsPerformed += microemulsion.performRandomSwap();
        ++swapAttempts;
        
        if (t >= lastOutputTime + dtOut)
        {
            // Writing this step to file
            logger.logEvent(PRODUCTION, t,
                    "Simulation summary: swapAttemps=%ld "
                    "| swapsPerformed=%ld "
                    "| swapRatio=%f",
                    swapAttempts, swapsPerformed, (double)swapsPerformed/swapAttempts);
            logger.logMsg(PRODUCTION, "Writing PGM file #%d", pgmWriter.getCounter());
            pgmWriter.write();
            pgmWriter.advanceSeries();
            // Advance output timer
            lastOutputTime += dtOut;
        }
        // Now finally advancing time
        t += dt;
    }
    
    // Final output
    logger.logEvent(PRODUCTION, t,
                    "Simulation summary: swapAttemps=%ld "
                    "| swapsPerformed=%ld "
                    "| swapRatio=%f",
                    swapAttempts, swapsPerformed, (double)swapsPerformed/swapAttempts);
    logger.logMsg(PRODUCTION, "Writing PGM file #%d", pgmWriter.getCounter());
    pgmWriter.write();
    
    //
    return 0;
}
