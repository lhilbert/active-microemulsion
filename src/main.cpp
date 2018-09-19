#include <iostream>
#include "Logger.h"
#include "Grid.h"
#include "Microemulsion.h"
#include "PgmWriter.h"
#include "ChainConfig.h"
#include "EventSchedule.h"
#include "EventSchedule.cpp" // Since template implementation is here
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace opt = boost::program_options;

void applyCutoffEvents(Logger &logger, EventSchedule<CutoffEvent> &eventSchedule, Microemulsion &microemulsion,
                       const std::set<ChainId> &allChains, const std::set<ChainId> &cutoffChains, double kChromPlus,
                       double kChromMinus, double kRnaPlus, double kRnaMinus, double t);

void takeSnapshots(Logger &logger, PgmWriter &dnaWriter, PgmWriter &rnaWriter, PgmWriter &transcriptionWriter, double t,
                   unsigned long swapAttempts, unsigned long swapsPerformed, unsigned long chemChangesPerformed,
                   bool isExtraSnapshot = false);

int main(int argc, const char **argv)
{
    std::string outputDir, inputImage, inputChainsFile;
    double endTime;
    double cutoffTime = -1;
    double cutoffTimeFraction = 1;
    int rows = 50, columns = 50;
    int swapsPerPixelPerUnitTime = 500;
    int numVisualizationOutputs = 100; //todo read this from config
    double snapshotInterval = -1;
    double extraSnapshotTimeOffset = -1;
    double omega = 0.5; //todo read this from config
    double kOn, kOff, kChromPlus, kChromMinus, kRnaPlus, kRnaMinus, kMax;
    std::set<double> kSet;
    
    std::vector<double> flavopiridolEvents;
    std::vector<double> actinomycinDEvents;
    std::vector<double> activationEvents;
    
    // Command-line argument parser. We use Boost Program Options (https://www.boost.org/doc/libs/1_68_0/doc/html/program_options.html).
    opt::options_description argsDescription("Supported options");
    // Here below some arguments, to be completed.
    argsDescription.add_options()
            ("help,h", "Show this help and exit")
            ("debug,d", "Enable the debug logging level")
            ("coarse-debug", "Enable the coarse_debug logging level")
            ("quiet,q", "Restrict logging to PRODUCTION,WARNING,ERROR levels")
            ("no-chain-integrity", "Do not enforce chain integrity")
            ("no-sticky-boundary", "Do not make boundary sticky to chromatin")
            ("flavopiridol",
             opt::value<std::vector<double>>(&flavopiridolEvents)->multitoken()->zero_tokens()->composing(),
             "Apply Flavopiridol at cutoff time(s). Cutoff time(s) can be specified as parameter")
            ("actinomycin-D",
             opt::value<std::vector<double>>(&actinomycinDEvents)->multitoken()->zero_tokens()->composing(),
             "Apply Actinomycin D at cutoff time. Cutoff time(s) can be specified as parameter")
            ("activate",
             opt::value<std::vector<double>>(&activationEvents)->multitoken()->zero_tokens()->composing(),
             "Activate transcription at cutoff time. Cutoff time(s) can be specified as parameter")
            ("output-dir,o", opt::value<std::string>(&outputDir)->default_value("./Out"),
             "Specify the folder to use for output (log and data)")
            ("input-image,i", opt::value<std::string>(&inputImage)->default_value(""),
             "Specify the image to be used as initial value for grid configuration")
            ("chains-config,P", opt::value<std::string>(&inputChainsFile)->default_value("testConfig.chains"),
             "Specify the chains config file to be used for grid configuration")
            ("end-time,T", opt::value<double>(&endTime)->default_value(1e3), "End time for the simulation")
            ("cutoff-time,C", opt::value<double>(&cutoffTime)->default_value(-1),
             "Time at which the chemical reaction cutoff takes place")
            ("cutoff-time-fraction,c", opt::value<double>(&cutoffTimeFraction)->default_value(1),
             "Fraction of endTime at which the chemical reaction cutoff takes place")
            ("snapshot-interval,t", opt::value<double>(&snapshotInterval)->default_value(-1),
             "Time interval (in seconds) between visualization snapshots. A negative time lets the '-S' flag take over")
            ("number-snapshots,S", opt::value<int>(&numVisualizationOutputs)->default_value(100),
             "Number of visualization snapshots to take. Only applied if '-t' is negative or not set")
            ("extra-snapshot,e", opt::value<double>(&extraSnapshotTimeOffset)->default_value(1800),
             "Time interval (in seconds) between cutoff events and their respective extra snapshot. A negative time disables the extra snapshot. The default value is 1800s = 30m")
//            ("all-extra-snapshots", "Enables the extra snapshot for all events (it is enabled only for last one by default)")
            ("width,W", opt::value<int>(&columns)->default_value(50), "Width of the simulation grid")
            ("height,H", opt::value<int>(&rows)->default_value(50), "Height of the simulation grid")
            ("omega,w", opt::value<double>(&omega)->default_value(0.5),
             "Energy cost for contiguity of non-affine species (omega model parameter)")
            ("sppps,s", opt::value<int>(&swapsPerPixelPerUnitTime)->default_value(333),
             "Number of average swap attempts per pixel per second")
            ("kOn", opt::value<double>(&kOn)->default_value(1.25e-3),
             "Reaction rate - Chromatin from non-transcribable to transcribable state")
            ("kOff", opt::value<double>(&kOff)->default_value(2.5e-3),
             "Reaction rate - Chromatin from transcribable to non-transcribable state")
            ("kChromPlus", opt::value<double>(&kChromPlus)->default_value(6.666666666e-3),
             "Reaction rate - Transcription turned ON")
            ("kChromMinus", opt::value<double>(&kChromMinus)->default_value(1.666666666e-3),
             "Reaction rate - Transcription turned OFF")
            ("kRnaPlus", opt::value<double>(&kRnaPlus)->default_value(8.333333333e-3),
             "Reaction rate - RBP from free to bound state")
            ("kRnaMinus", opt::value<double>(&kRnaMinus)->default_value(8.333333333e-4),
             "Reaction rate - RBP from bound to free state");
    opt::variables_map varsMap;
    opt::store(opt::parse_command_line(argc, argv, argsDescription), varsMap);
    opt::notify(varsMap);
    
    if (varsMap.count("help"))
    {
        std::cout << argsDescription << std::endl;
        return 1;
    }

//    // debug
//    std::cout << flavopiridolEvents.size() << std::endl;
//    return 1;
//    //
    
    bool debugMode = varsMap.count("debug") > 0;
    bool coarseDebugMode = varsMap.count("coarse-debug") > 0;
    bool quietMode = varsMap.count("quiet") > 0;
    bool enforceChainIntegrity = varsMap.count("no-chain-integrity") == 0;
    bool stickyBoundary = varsMap.count("no-sticky-boundary") == 0;
    bool flavopiridolSwitchPassed = varsMap.count("flavopiridol") > 0;
    bool actinomycinDSwitchPassed = varsMap.count("actinomycin-D") > 0;
    bool activateSwitchPassed = varsMap.count("activate") > 0;
//    bool allExtraSnapshots = varsMap.count("all-extra-snapshots") > 0;
    bool allExtraSnapshots = false;
    //
    int numInnerCells = rows * columns;
    double dt = 1.0 / (double) (swapsPerPixelPerUnitTime * numInnerCells); // The dt used for timestepping.
    kSet.insert(kOn);
    kSet.insert(kOff);
    kSet.insert(kChromPlus);
    kSet.insert(kChromMinus);
    kSet.insert(kRnaPlus);
    kSet.insert(kRnaMinus);
    kMax = *kSet.rbegin(); // Get the maximum on the set
    double dtChem = 0.1 / kMax;
    if (snapshotInterval <= 0) // Auto-compute it only if it was not set
    {
        snapshotInterval =
                endTime / numVisualizationOutputs; // The dt used for visualization output.
    }
    else // If instead the interval is given, recompute the num snapshot for logging purpose
    {
        numVisualizationOutputs = static_cast<int>(floor(endTime / snapshotInterval));
    }
    if (cutoffTime < 0)
    {
        cutoffTime = endTime / cutoffTimeFraction;
    }
    
    // Populate the events' schedule
    EventSchedule<CutoffEvent> cutoffSchedule(cutoffTime);
    if (flavopiridolSwitchPassed)
    {
        cutoffSchedule.addEvents(flavopiridolEvents, FLAVOPIRIDOL);
    }
    if (actinomycinDSwitchPassed)
    {
        cutoffSchedule.addEvents(actinomycinDEvents, ACTINOMYCIN_D);
    }
    if (activateSwitchPassed)
    {
        cutoffSchedule.addEvents(activationEvents, ACTIVATE);
    }
    
    // Populate the extra snapshots' schedule
    EventSchedule<SnapshotEvent> extraSnapshotSchedule(cutoffTime);
    if (extraSnapshotTimeOffset >= 0 && extraSnapshotTimeOffset < endTime)
    {
        if (allExtraSnapshots)
        {
            //todo
            double lastEventTime = cutoffSchedule.getLastEventTime();
            extraSnapshotSchedule.addEvent(lastEventTime + extraSnapshotTimeOffset, GENERIC_SNAPSHOT); //placeholder
        }
        else
        {
            double lastEventTime = cutoffSchedule.getLastEventTime();
            extraSnapshotSchedule.addEvent(lastEventTime + extraSnapshotTimeOffset, GENERIC_SNAPSHOT);
        }
    }
    
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
    else if (coarseDebugMode)
    {
        logger.setDebugLevel(COARSE_DEBUG);
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
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(coarseDebugMode));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(quietMode));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(enforceChainIntegrity));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(stickyBoundary));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(flavopiridolSwitchPassed));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(actinomycinDSwitchPassed));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(activateSwitchPassed));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(cutoffSchedule.size()));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%s", DUMP(outputDir.data()));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%s", DUMP(inputImage.data()));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%s", DUMP(inputChainsFile.data()));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%.2e", DUMP(endTime));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%.2e", DUMP(cutoffTime));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(rows));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(columns));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(swapsPerPixelPerUnitTime));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(omega));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kOn));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kOff));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kChromPlus));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kChromMinus));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kRnaPlus));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kRnaMinus));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kMax));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(numVisualizationOutputs));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(extraSnapshotTimeOffset));
    
    logger.logMsg(logger.getDebugLevel(), "Timers logging: %s=%f", DUMP(dt));
    logger.logMsg(logger.getDebugLevel(), "Timers logging: %s=%f", DUMP(dtChem));
    logger.logMsg(logger.getDebugLevel(), "Timers logging: %s=%f", DUMP(snapshotInterval));
    
    // Config-file parser
    //logger.logMsg(PRODUCTION, "Reading configuration");
    //todo Actually support config files
    
    // Initialize data structures...
    logger.logMsg(PRODUCTION, "Reading polymeric chains configuration");
    std::set<ChainId> allChains, cutoffChains;
    Grid grid(columns, rows, logger);
    grid.initializeInnerGridAs(Grid::chemicalPropertiesOf(RBP, NOT_ACTIVE));
    // ...and read chains configuration
    ChainConfig chainConfig(logger);
    std::ifstream chainConfigFile(inputChainsFile);
    while (chainConfigFile >> chainConfig)
    {
        const std::map<std::string, unsigned char> &chainProperties = chainConfig.getChainProperties();
        int column = chainConfig.getStartCol(), row = chainConfig.getStartRow();
        ChemicalProperties chemicalProperties = Grid::chemicalPropertiesOf(CHROMATIN,
                                                                           static_cast<Activity>(chainConfig.isActive()));
        Flags flags = Grid::flagsOf(static_cast<Transcribability>(chainConfig.isTranscribable()),
                                    static_cast<TranscriptionInhibition>(!chainConfig.isInhibited()));
        auto newChain = grid.initializeGridWithStepInstructions(allChains, column, row, chainConfig.getSteps(),
                                                                chemicalProperties, flags);
        
        auto pos = chainProperties.find("Cutoff");
        if (pos != chainProperties.end() && pos->second != 0)
        {
            cutoffChains.insert(newChain.begin(), newChain.end());
        }
    }
    
    int numRbpCells = grid.getSpeciesCount(RBP);
    int numChromatinCells = numInnerCells - numRbpCells;
    double rbpRatio = static_cast<double>(numRbpCells) / numInnerCells;
    double chromatinRatio = static_cast<double>(numChromatinCells) / numInnerCells;
    logger.logMsg(PRODUCTION, "GRID: %s=%d, %s=%d, %s=%.3f, %s=%.3f", DUMP(numChromatinCells), DUMP(numRbpCells),
                  DUMP(chromatinRatio), DUMP(rbpRatio));
    logger.logMsg(PRODUCTION, "CHAINS: %s=%d, %s=%d", DUMP(allChains.size()), DUMP(cutoffChains.size()));
    logger.logMsg(PRODUCTION, "Initializing microemulsion: %s=%f, %s=%f, %s=%f, %s=%f, %s=%f, %s=%f, %s=%f",
                  DUMP(dtChem), DUMP(kOn), DUMP(kOff), DUMP(kChromPlus), DUMP(kChromMinus), DUMP(kRnaPlus),
                  DUMP(kRnaMinus));
    Microemulsion microemulsion(grid, omega, logger,
                                dtChem, kOn, kOff, kChromPlus, kChromMinus, kRnaPlus, kRnaMinus, stickyBoundary);
    
    // Initialize PgmWriters for the 3 channels
    PgmWriter dnaWriter(logger, columns, rows, outputDir + "/microemulsion_DNA", "DNA",
                        Grid::isChromatin);
    PgmWriter rnaWriter(logger, columns, rows, outputDir + "/microemulsion_RNA", "RNA",
                        [](ChemicalProperties chemicalProperties) {
                            return Grid::isActiveRBP(chemicalProperties)
                                   || Grid::isActiveChromatin(chemicalProperties);
                        });
    PgmWriter transcriptionWriter(logger, columns, rows, outputDir + "/microemulsion_Transcription", "Pol II Ser2Phos",
                                  Grid::isActiveChromatin);
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
    double nextOutputTime = snapshotInterval;
    double nextChemTime = dtChem;
    unsigned long swapAttempts = 0;
    unsigned long swapsPerformed = 0;
    unsigned long chemChangesPerformed = 0;
    logger.logEvent(INFO, t, "Entering main time-stepping loop");
    while (t < endTime)
    {
        if (cutoffSchedule.check(t))
        {
            applyCutoffEvents(logger, cutoffSchedule, microemulsion,
                              allChains, cutoffChains,
                              kChromPlus, kChromMinus, kRnaPlus, kRnaMinus,
                              t);
        }
        if (extraSnapshotSchedule.check(t))
        {
            extraSnapshotSchedule.popEventsToApply(
                    t); // Throwing the return value away since we are just going to take 1 extra snapshot per time
            takeSnapshots(logger, dnaWriter, rnaWriter, transcriptionWriter, t, swapAttempts,
                          swapsPerformed, chemChangesPerformed, true);
        }
        // Time-stepping loop
        while (t < nextOutputTime)
        {
            t += dt;
            swapsPerformed += microemulsion.performRandomSwap();
            ++swapAttempts;
            
            // Now check if to perform chemical reactions
            if (t >= nextChemTime)
            {
                chemChangesPerformed += microemulsion.performChemicalReactions();
                nextChemTime += dtChem;
            }
        }
        
        // Writing this step to file
        takeSnapshots(logger, dnaWriter, rnaWriter, transcriptionWriter, t, swapAttempts, swapsPerformed,
                      chemChangesPerformed);
        // Advance output timer
        nextOutputTime += snapshotInterval;
    }
    logger.logEvent(DEBUG, t, "Exiting main time-stepping loop");
    
    //
    return 0;
}

void applyCutoffEvents(Logger &logger, EventSchedule<CutoffEvent> &eventSchedule, Microemulsion &microemulsion,
                       const std::set<ChainId> &allChains, const std::set<ChainId> &cutoffChains, double kChromPlus,
                       double kChromMinus, double kRnaPlus, double kRnaMinus, double t)
{
    auto eventsToApply = eventSchedule.popEventsToApply(t);
    for (auto event : eventsToApply)
    {
        if (event == FLAVOPIRIDOL)
        {
            logger.logEvent(PRODUCTION, t, "CUTOFF: Applying Flavopiridol condition");
            microemulsion.setKChromPlus(0);
        }
        else if (event == ACTINOMYCIN_D)
        {
            logger.logEvent(PRODUCTION, t, "CUTOFF: Applying Actinomycin D condition");
            microemulsion.setKChromPlus(0);
            microemulsion.setKChromMinus(0);
            microemulsion.setKRnaPlus(0);
        }
        else if (event == ACTIVATE)
        {
            logger.logEvent(PRODUCTION, t, "CUTOFF: Activating transcription");
            microemulsion.setKChromPlus(kChromPlus);
            microemulsion.setKChromMinus(kChromMinus);
            microemulsion.setKRnaPlus(kRnaPlus);
            microemulsion.setKRnaMinus(kRnaMinus);
        }
        else
        {
            // Testing playground here...
            logger.logEvent(PRODUCTION, t, "CUTOFF: Applying custom cutoff conditions");
//            microemulsion.setKRnaMinus(0);
            microemulsion.setTranscriptionInhibitionOnChains(allChains, TRANSCRIPTION_POSSIBLE);
            microemulsion.setTranscriptionInhibitionOnChains(cutoffChains, TRANSCRIPTION_INHIBITED);
        }
    }
}

void takeSnapshots(Logger &logger, PgmWriter &dnaWriter, PgmWriter &rnaWriter, PgmWriter &transcriptionWriter, double t,
                   unsigned long swapAttempts, unsigned long swapsPerformed, unsigned long chemChangesPerformed,
                   bool isExtraSnapshot)
{
    logger.logEvent(PRODUCTION, t,
                    "Simulation summary: %s=%ld "
                    "| %s=%ld "
                    "| swapRatio=%f "
                    "| %s=%ld ",
                    DUMP(swapAttempts), DUMP(swapsPerformed),
                    (double) swapsPerformed / swapAttempts,
                    DUMP(chemChangesPerformed));
    dnaWriter.write(t, isExtraSnapshot);
    rnaWriter.write(t, isExtraSnapshot);
    transcriptionWriter.write(t, isExtraSnapshot);
    if (!isExtraSnapshot)
    {
        dnaWriter.advanceSeries();
        rnaWriter.advanceSeries();
        transcriptionWriter.advanceSeries();
    }
}

//eof
