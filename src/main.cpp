#include <iostream>
#include "Logger.h"
#include "Grid.h"
#include "Microemulsion.h"
#include "PgmWriter.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace opt = boost::program_options;

int main(int argc, const char **argv)
{
    std::string outputDir, inputImage;
    double endTime;
    
    // Command-line argument parser. We use Boost Program Options (https://www.boost.org/doc/libs/1_68_0/doc/html/program_options.html).
    opt::options_description argsDescription("Supported options");
    // Here below some arguments, to be completed.
    argsDescription.add_options()
        ("help,h", "Show this help and exit")
        ("debug,d", "Enable the debug logging level")
        ("quiet,q", "Restrict logging to PRODUCTION,WARNING,ERROR levels")
        ("output-dir,o", opt::value<std::string>(&outputDir)->default_value("./Out"),
                "Specify the folder to use for output (log and data)")
        ("input-image,i", opt::value<std::string>(&inputImage),
                "Specify the image to be used as initial value for grid configuration")
        ("end-time,T", opt::value<double>(&endTime)->default_value(1e3), "End time for the simulation")
    ;
    opt::variables_map varsMap;
    opt::store(opt::parse_command_line(argc, argv, argsDescription), varsMap);
    opt::notify(varsMap);
    
    if (varsMap.count("help"))
    {
        std::cout << argsDescription << std::endl;
        return 1;
    }
    
    bool debugMode = varsMap.count("debug")>0;
    bool quietMode = varsMap.count("quiet")>0;
    int rows = 50, columns = 50; //todo: then read these from configuration
//    int endTime = argparser.retrieve<int>("end-time");
    int swapsPerPixelPerUnitTime = 80; //todo read this from config
    int numVisualizationOutputs = 100; //todo read this from config
    double omega = 0.5; //todo read this from config
    //
    int numInnerCells = rows * columns;
    double dt = 1.0 / (double)(swapsPerPixelPerUnitTime * numInnerCells); // The dt used for timestepping.
    double dtOut = endTime/numVisualizationOutputs; // The dt used for visualization output. //todo read this from config
    
    // If output folder doesn't exist, create it
    if (!boost::filesystem::exists(outputDir))
    {
        boost::filesystem::create_directory(outputDir);
    }
    // Setup and start Logger
    Logger logger;
    logger.setOutputFolder(outputDir.data());
    if (debugMode)
    {
        logger.setDebugLevel(DEBUG);
    }
    else if (quietMode)
    {
        logger.setDebugLevel(PRODUCTION);
    }
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
    PgmWriter pgmWriter(columns, rows, 1, outputDir+"/microemulsion");
    pgmWriter.setData(grid.getData());
    // Write initial data to file
    logger.logMsg(PRODUCTION, "Writing PGM file #%d (%s)", pgmWriter.getCounter(),
                  pgmWriter.getOutputFileFullNameCstring());
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
                            swapAttempts, swapsPerformed, (double) swapsPerformed / swapAttempts);
            logger.logMsg(PRODUCTION, "Writing PGM file #%d (%s)", pgmWriter.getCounter(),
                          pgmWriter.getOutputFileFullNameCstring());
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
                    swapAttempts, swapsPerformed, (double) swapsPerformed / swapAttempts);
    logger.logMsg(PRODUCTION, "Writing PGM file #%d (%s)", pgmWriter.getCounter(),
                  pgmWriter.getOutputFileFullNameCstring());
    pgmWriter.write();
    
    //
    return 0;
}
