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
    int rows = 50, columns = 50; //todo: then read these from config
    int swapsPerPixelPerUnitTime = 500; //todo read this from config
    int numVisualizationOutputs = 100; //todo read this from config
    double omega = 0.5; //todo read this from config
    double kOn, kOff, kChromPlus, kChromMinus, kRnaPlus, kRnaMinus, kMax;
    std::set<double> kSet;
    
    // Command-line argument parser. We use Boost Program Options (https://www.boost.org/doc/libs/1_68_0/doc/html/program_options.html).
    opt::options_description argsDescription("Supported options");
    // Here below some arguments, to be completed.
    argsDescription.add_options()
        ("help,h", "Show this help and exit")
        ("debug,d", "Enable the debug logging level")
        ("quiet,q", "Restrict logging to PRODUCTION,WARNING,ERROR levels")
        ("output-dir,o", opt::value<std::string>(&outputDir)->default_value("./Out"),
                "Specify the folder to use for output (log and data)")
        ("input-image,i", opt::value<std::string>(&inputImage)->default_value(""),
                "Specify the image to be used as initial value for grid configuration")
        ("end-time,T", opt::value<double>(&endTime)->default_value(1e3), "End time for the simulation")
        ("width,W", opt::value<int>(&columns)->default_value(50), "Width of the simulation grid")
        ("height,H", opt::value<int>(&rows)->default_value(50), "Height of the simulation grid")
        ("omega,w", opt::value<double>(&omega)->default_value(0.5), "Energy cost for contiguity of non-affine species (omega model parameter)")
        ("sppps,s", opt::value<int>(&swapsPerPixelPerUnitTime)->default_value(333), "Number of average swap attempts per pixel per second")
        ("kOn", opt::value<double>(&kOn)->default_value(1.25e-3), "Reaction rate - Chromatin from non-transcribable to transcribable state")
        ("kOff", opt::value<double>(&kOff)->default_value(2.5e-3), "Reaction rate - Chromatin from transcribable to non-transcribable state")
        ("kChromPlus", opt::value<double>(&kChromPlus)->default_value(6.666666666e-3), "Reaction rate - Transcription turned ON")
        ("kChromMinus", opt::value<double>(&kChromMinus)->default_value(1.666666666e-3), "Reaction rate - Transcription turned OFF")
        ("kRnaPlus", opt::value<double>(&kRnaPlus)->default_value(8.333333333e-3), "Reaction rate - RBP from free to bound state")
        ("kRnaMinus", opt::value<double>(&kRnaMinus)->default_value(8.333333333e-4), "Reaction rate - RBP from bound to free state")
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
//    int endTime = argparser.retrieve<int>("end-time");
    //
    int numInnerCells = rows * columns;
    double dt = 1.0 / (double)(swapsPerPixelPerUnitTime * numInnerCells); // The dt used for timestepping.
    kSet.insert(kOn); kSet.insert(kOff);
    kSet.insert(kChromPlus); kSet.insert(kChromMinus);
    kSet.insert(kRnaPlus); kSet.insert(kRnaMinus);
    kMax = *kSet.rbegin(); // Get the maximum on the set
    double dtChem = 0.1 / kMax;
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
    logger.logArgv(argc, argv); // Logging invocation command.
    // Logging parameters for this run
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: Debug level set to %s", logger.getDebugLevelStr());
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(debugMode));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(quietMode));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%s", DUMP(outputDir.data()));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%s", DUMP(inputImage.data()));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%.2e", DUMP(endTime));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(rows));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(columns));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(swapsPerPixelPerUnitTime));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(numVisualizationOutputs));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(omega));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kOn));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kOff));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kChromPlus));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kChromMinus));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kRnaPlus));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kRnaMinus));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kMax));
    
    logger.logMsg(logger.getDebugLevel(), "Timers logging: %s=%f", DUMP(dt));
    logger.logMsg(logger.getDebugLevel(), "Timers logging: %s=%f", DUMP(dtChem));
    logger.logMsg(logger.getDebugLevel(), "Timers logging: %s=%f", DUMP(dtOut));

    // Config-file parser
    //logger.logMsg(PRODUCTION, "Reading configuration");
    //todo
    
    // Initialize data structures
    Grid grid(columns, rows, logger);
//    grid.initializeInnerGridAs(CHROMATINE, NOT_ACTIVE);
//    grid.initializeGridRandomly(0.5, RBP, ACTIVE);
    grid.initializeInnerGridAs(RBP, ACTIVE);
//    grid.initializeGridWithSingleChain(RBP, ACTIVE);
//    grid.initializeGridWithTwoParallelChains(5, RBP, ACTIVE);
//    grid.initializeGridWithTwoParallelChains(10, RBP, ACTIVE);
    grid.initializeGridWithTwoOrthogonalChains(0, 0, CHROMATINE, NOT_ACTIVE);
//    grid.initializeGridWithTwoOrthogonalChains(-5, -5, RBP, ACTIVE);
//    grid.initializeGridWithTwoOrthogonalChains(+5, +5, RBP, ACTIVE);
    int numRbpCells = grid.getSpeciesCount(RBP);
    int numChromatineCells = (rows * columns) - numRbpCells;
    double speciesRatio = static_cast<double>(numRbpCells) / numChromatineCells;
    logger.logMsg(PRODUCTION, "GRID: %s=%d, %s=%d, %s=%.3f", DUMP(numChromatineCells), DUMP(numRbpCells), DUMP(speciesRatio));
    logger.logMsg(PRODUCTION, "Initializing microemulsion: %s=%f, %s=%f, %s=%f, %s=%f, %s=%f, %s=%f, %s=%f",
                  DUMP(dtChem), DUMP(kOn), DUMP(kOff), DUMP(kChromPlus), DUMP(kChromMinus), DUMP(kRnaPlus), DUMP(kRnaMinus));
    Microemulsion microemulsion(grid, omega, logger,
            dtChem, kOn, kOff, kChromPlus, kChromMinus, kRnaPlus, kRnaMinus);
    
    // Initialize PgmWriters for the 3 channels
    PgmWriter dnaWriter(logger, columns, rows, outputDir + "/microemulsion_DNA", "DNA",
                        Grid::chemicalPropertiesOf(CHROMATINE, NOT_ACTIVE));
    PgmWriter rnaWriter(logger, columns, rows, outputDir + "/microemulsion_RNA", "RNA",
                        Grid::chemicalPropertiesOf(RBP, ACTIVE));
    PgmWriter transcriptionWriter(logger, columns, rows, outputDir + "/microemulsion_Transcription", "Pol II Ser2Phos",
                        Grid::chemicalPropertiesOf(CHROMATINE, ACTIVE));
    dnaWriter.setData(grid.getData());
    rnaWriter.setData(grid.getData());
    transcriptionWriter.setData(grid.getData());
    
    // Simulation loops
    double t = 0;
    // Write initial data to file
    dnaWriter.write(t);
    rnaWriter.write(t);
    transcriptionWriter.write(t);
    dnaWriter.advanceSeries();
    rnaWriter.advanceSeries();
    transcriptionWriter.advanceSeries();
    //
    double lastOutputTime = 0;
    unsigned long swapAttempts = 0;
    unsigned long swapsPerformed = 0;
    logger.logEvent(INFO, t, "Entering main time-stepping loop");
    while (t < endTime)
    {
        // Time-stepping loop
        while (t < lastOutputTime + dtOut)
        {
            t += dt;
            swapsPerformed += microemulsion.performRandomSwap();
            ++swapAttempts;
        }
    
        // Writing this step to file
        //todo: Extend info logged in simulation summary to include chem reaction
        logger.logEvent(PRODUCTION, t,
                        "Simulation summary: swapAttemps=%ld "
                        "| swapsPerformed=%ld "
                        "| swapRatio=%f",
                        swapAttempts, swapsPerformed, (double) swapsPerformed / swapAttempts);
        dnaWriter.write(t);
        rnaWriter.write(t);
        transcriptionWriter.write(t);
        dnaWriter.advanceSeries();
        rnaWriter.advanceSeries();
        transcriptionWriter.advanceSeries();
        // Advance output timer
        lastOutputTime += dtOut;
    }
    logger.logEvent(DEBUG, t, "Exiting main time-stepping loop");
//    logger.logEvent(DEBUG, t, "%s=%f", DUMP(lastOutputTime));

//    Below should not be required. //todo: remove this part if really useless
//    // Final output, but make sure we didn't just write in the last iteration!
//    if (t > lastOutputTime)
//    {
//        logger.logEvent(PRODUCTION, t,
//                        "Simulation summary: swapAttemps=%ld "
//                        "| swapsPerformed=%ld "
//                        "| swapRatio=%f",
//                        swapAttempts, swapsPerformed, (double) swapsPerformed / swapAttempts);
//        dnaWriter.write(t);
//        rnaWriter.write(t);
//        transcriptionWriter.write(t);
//    }
    
    //
    return 0;
}
