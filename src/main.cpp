#include <iostream>
#include "Logger/Logger.h"
#include "Grid/Grid.h"
#include "Microemulsion/Microemulsion.h"
#include "Visualization/PgmWriter.h"
#include "Chain/ChainConfig.h"
#include "EventSchedule/EventSchedule.h"
#include "EventSchedule/EventSchedule.cpp" // Since template implementation is here
#include "Grid/GridInitializer.h"
#include "Cell/CellData.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace opt = boost::program_options;

void applyCutoffEvents(Logger &logger, EventSchedule<CutoffEvent> &eventSchedule, Microemulsion &microemulsion,
                       const std::set<ChainId> &allChains, const std::set<ChainId> &cutoffChains,
                       const std::set<ChainId> &permissibleChains, double kOn, double kOff, double kChromPlus,
                       double kChromMinus, double kRnaPlus, double kRnaMinus, double kRnaTransfer, double t);

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
    int numThreads = 1;
    unsigned int swapRounds = 0;
    int swapsPerPixelPerUnitTime = 500;
    int numVisualizationOutputs = 100; //todo read this from config
    double snapshotInterval = -1;
    double extraSnapshotTimeOffset = -1;
    double extraSnapshotTimeAbs = -1;
    double omega = 0.33; //todo read this from config
    double kOn, kOff, kChromPlus, kChromMinus, kRnaPlus, kRnaMinus, kRnaTransfer, kMax;
    std::set<double> kSet;
    
    std::vector<double> flavopiridolEvents;
    std::vector<double> actinomycinDEvents;
    std::vector<double> activationEvents;
    std::vector<double> txnSpikeEvents;
    std::vector<double> additionalExplicitSnapshots;
    
    // Command-line argument parser. We use Boost Program Options (https://www.boost.org/doc/libs/1_68_0/doc/html/program_options.html).
    opt::options_description argsDescription("Supported options");
    // Here below some arguments, to be completed.
    argsDescription.add_options()
            ("help,h", "Show this help and exit")
            ("debug,d", "Enable the debug logging level")
            ("coarse-debug", "Enable the coarse_debug logging level")
            ("quiet,q", "Restrict logging to PRODUCTION,WARNING,ERROR levels")
            ("Quiet,Q", "Restrict logging to WARNING,ERROR levels")
            ("minutes,m", "Time variables are expressed in minutes instead of seconds")
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
            ("txn-spike",
             opt::value<std::vector<double>>(&txnSpikeEvents)->multitoken()->zero_tokens()->composing(),
             "Set a transcription spike at cutoff time. Cutoff time(s) can be specified as parameter")
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
            ("extra-snapshot,e", opt::value<double>(&extraSnapshotTimeOffset)->default_value(-1),
             "Time interval (in seconds) between cutoff events and their respective extra snapshot. "
             "A negative time disables the extra snapshot.")
            ("extra-snapshot-abs,E", opt::value<double>(&extraSnapshotTimeAbs)->default_value(-1),
             "Time (in seconds) when the extra snapshot should be taken. A negative time disables this setting. This overrides -e.")
//            ("all-extra-snapshots", "Enables the extra snapshot for all events (it is enabled only for last one by default)")
            ("additional-snapshots",
             opt::value<std::vector<double>>(&additionalExplicitSnapshots)->multitoken()->zero_tokens()->composing(),
             "Explicitly add additional snapshot time(s). Snapshot time(s) can be specified as parameter (space-separated)")
            ("width,W", opt::value<int>(&columns)->default_value(50), "Width of the simulation grid")
            ("height,H", opt::value<int>(&rows)->default_value(50), "Height of the simulation grid")
            ("threads", opt::value<int>(&numThreads)->default_value(-1),
             "Number of threads to use for parallelization. A negative value lets OMP_NUM_THREADS take precedence")
            ("omega,w", opt::value<double>(&omega)->default_value(0.33),
             "Energy cost for contiguity of non-affine species (omega model parameter)")
            ("sppps,s", opt::value<int>(&swapsPerPixelPerUnitTime)->default_value(4500),
             "Number of average swap attempts per pixel per second")
            ("kOn", opt::value<double>(&kOn)->default_value(2.5e-4),
             "Reaction rate - Chromatin from non-transcribable to transcribable state")
            ("kOff", opt::value<double>(&kOff)->default_value(3.3333e-3),
             "Reaction rate - Chromatin from transcribable to non-transcribable state")
            ("kChromPlus", opt::value<double>(&kChromPlus)->default_value(1.5e-3),
             "Reaction rate - Transcription turned ON")
            ("kChromMinus", opt::value<double>(&kChromMinus)->default_value(1.6666e-3),
             "Reaction rate - Transcription turned OFF")
            ("kRnaPlus", opt::value<double>(&kRnaPlus)->default_value(3e-1),
             "Reaction rate - RBP from free to bound state")
            ("kRnaMinusRbp", opt::value<double>(&kRnaMinus)->default_value(8.3333e-4),
             "Reaction rate - RBP from bound to free state")
            ("kRnaTransfer", opt::value<double>(&kRnaTransfer)->default_value(3.3e-3),
             "Reaction rate - RNA migrating from transcription site to an RBP site");
    opt::variables_map varsMap;
    opt::store(opt::parse_command_line(argc, argv, argsDescription), varsMap);
    opt::notify(varsMap);
    
    if (varsMap.count("help"))
    {
        std::cout << argsDescription << std::endl;
        return 1;
    }
    
    if (numThreads < 0)
    {
        numThreads = omp_get_num_threads();
    }
    else
    {
        omp_set_num_threads(numThreads);
    }
    
    bool debugMode = varsMap.count("debug") > 0;
    bool coarseDebugMode = varsMap.count("coarse-debug") > 0;
    bool quietMode = varsMap.count("quiet") > 0;
    bool QuietMode = varsMap.count("Quiet") > 0;
    bool enforceChainIntegrity = varsMap.count("no-chain-integrity") == 0;
    bool stickyBoundary = varsMap.count("no-sticky-boundary") == 0;
    bool flavopiridolSwitchPassed = varsMap.count("flavopiridol") > 0;
    bool actinomycinDSwitchPassed = varsMap.count("actinomycin-D") > 0;
    bool activateSwitchPassed = varsMap.count("activate") > 0;
    bool txnSpikeSwitchPassed = varsMap.count("txn-spike") > 0;
    bool isTimeInMinutes = varsMap.count("minutes") > 0;
//    bool allExtraSnapshots = varsMap.count("all-extra-snapshots") > 0;
    bool allExtraSnapshots = false;
    bool additionalSnapshotsPassed = varsMap.count("additional-snapshots") > 0;
    //
    double timeMultiplier = 1;
    if (isTimeInMinutes)
    {
        timeMultiplier = 60;
    }
    endTime *= timeMultiplier;
    snapshotInterval *= timeMultiplier;
    extraSnapshotTimeOffset *= timeMultiplier;
    extraSnapshotTimeAbs *= timeMultiplier;
    cutoffTime *= timeMultiplier;
    
    // Start timers computation
    kSet.insert(kOn);
    kSet.insert(kOff);
    kSet.insert(kChromPlus);
    kSet.insert(kChromMinus);
    kSet.insert(kRnaPlus);
    kSet.insert(kRnaMinus);
    kSet.insert(kRnaTransfer);
    kMax = *kSet.rbegin(); // Get the maximum on the set
    //dtChem
    double dtChem = 0.1 / kMax;
    //snapshotInterval
    if (snapshotInterval <= 0) // Auto-compute it only if it was not set
    {
        snapshotInterval =
                endTime / numVisualizationOutputs; // The dt used for visualization output.
    }
    else // If instead the interval is given, recompute the num snapshot for logging purpose
    {
        numVisualizationOutputs = static_cast<int>(floor(endTime / snapshotInterval));
    }
    //dt
    int numInnerCells = rows * columns;
//    double dt = 1.0 / (double) (swapsPerPixelPerUnitTime * numInnerCells); // The dt used for timestepping.
    int cellsPerColour = numInnerCells / (Microemulsion::colourStride * Microemulsion::colourStride);
    
    // Compute swaps round if required
    double alpha = (double) (swapsPerPixelPerUnitTime * Microemulsion::colourStride * Microemulsion::colourStride);
    if (swapRounds == 0)
    {
        int safetyFactor = 3;
        double k = fmin(dtChem, snapshotInterval);
        swapRounds = static_cast<unsigned int>(ceil(k * alpha / safetyFactor));
    }
    double dt = swapRounds * 1.0 / alpha; // The dt used for timestepping.
    //
    
    if (cutoffTime < 0)
    {
        cutoffTime = endTime / cutoffTimeFraction;
    }
    
    // Populate the events' schedule
    EventSchedule<CutoffEvent> cutoffSchedule(cutoffTime);
    if (flavopiridolSwitchPassed)
    {
        cutoffSchedule.addEvents(flavopiridolEvents, FLAVOPIRIDOL, timeMultiplier);
    }
    if (actinomycinDSwitchPassed)
    {
        cutoffSchedule.addEvents(actinomycinDEvents, ACTINOMYCIN_D, timeMultiplier);
    }
    if (activateSwitchPassed)
    {
        cutoffSchedule.addEvents(activationEvents, ACTIVATE, timeMultiplier);
    }
    if (txnSpikeSwitchPassed)
    {
        cutoffSchedule.addEvents(txnSpikeEvents, TXN_SPIKE, timeMultiplier);
        // It seems wise not to auto-reactivate (not yet) since it may take quite a long time for RNA to be produced.
        // So an activate event has to be passed to specify when to resume with normal rates.
//        std::vector<double> reactivationEvents;
//        for (auto evt : txnSpikeEvents)
//        {
//            reactivationEvents.push_back(evt + 2*dtChem); //At t + 2*dtChem we reactivate, so it is reset normally after 2 rounds
//        }
//        cutoffSchedule.addEvents(reactivationEvents, ACTIVATE, timeMultiplier);
    }
    
    // Populate the snapshots' schedule
    EventSchedule<SnapshotEvent> snapshotSchedule(cutoffTime);
    snapshotSchedule.addEvents(snapshotInterval, endTime, snapshotInterval,
                               NORMAL_SNAPSHOT); // No time multiplier required here as snapshotInterval is already rescaled
    // Now add manually specified snapshots to the scheduled
    if (additionalSnapshotsPassed)
    {
        snapshotSchedule.addEvents(additionalExplicitSnapshots, NORMAL_SNAPSHOT, timeMultiplier);
    }
    // Add extra snapshots
    if (extraSnapshotTimeAbs >= 0 && extraSnapshotTimeAbs <= endTime)
    {
        snapshotSchedule.addEvent(extraSnapshotTimeAbs, GENERIC_EXTRA_SNAPSHOT);
    }
    else if (extraSnapshotTimeOffset >= 0 && extraSnapshotTimeOffset <= endTime && cutoffSchedule.size() > 0)
    {
        if (allExtraSnapshots)
        {
            //todo
            double lastEventTime = cutoffSchedule.getLastEventTime();
            snapshotSchedule.addEvent(lastEventTime + extraSnapshotTimeOffset, GENERIC_EXTRA_SNAPSHOT); //placeholder
        }
        else
        {
            double lastEventTime = cutoffSchedule.getLastEventTime();
            snapshotSchedule.addEvent(lastEventTime + extraSnapshotTimeOffset, GENERIC_EXTRA_SNAPSHOT);
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
    else if (QuietMode)
    {
        logger.setDebugLevel(WARNING);
    }
    logger.openLogFile();
    logger.setStartTime();
    logger.logArgv(argc, argv); // Logging invocation command.
    // Logging parameters for this run
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: Debug level set to %s", logger.getDebugLevelStr());
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(debugMode));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(coarseDebugMode));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(quietMode));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(QuietMode));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(enforceChainIntegrity));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(stickyBoundary));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(isTimeInMinutes));
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
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kRnaTransfer));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%f", DUMP(kMax));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(numVisualizationOutputs));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(extraSnapshotTimeOffset));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(numThreads));
    logger.logMsg(logger.getDebugLevel(), "Parameters logging: %s=%d", DUMP(swapRounds));
    
    logger.logMsg(logger.getDebugLevel(), "Timers logging: %s=%f", DUMP(endTime));
    logger.logMsg(logger.getDebugLevel(), "Timers logging: %s=%f", DUMP(dt));
    logger.logMsg(logger.getDebugLevel(), "Timers logging: %s=%f", DUMP(dtChem));
    logger.logMsg(logger.getDebugLevel(), "Timers logging: %s=%f", DUMP(snapshotInterval));
    
    // Config-file parser
    //logger.logMsg(PRODUCTION, "Reading configuration");
    //todo Actually support config files
    
    // Initialize data structures...
    logger.logMsg(PRODUCTION, "Reading polymeric chains configuration");
    std::set<ChainId> allChains, cutoffChains, permissibleChains;
    Grid grid(columns, rows, logger);
    GridInitializer::initializeInnerGridAs(grid, CellData::chemicalPropertiesOf(RBP, NOT_ACTIVE));
    if (!stickyBoundary) {
        GridInitializer::initializeOuterGridAs(grid, CellData::chemicalPropertiesOf(RBP, NOT_ACTIVE));
    }
    // ...and read chain configuration file, construct chain structure from that file
    ChainConfig chainConfig(logger);
    std::ifstream chainConfigFile(inputChainsFile);
    while (chainConfigFile >> chainConfig)
    {
        const std::map<std::string, unsigned char> &chainProperties = chainConfig.getChainProperties();
        int column = chainConfig.getStartCol(), row = chainConfig.getStartRow();
        ChemicalProperties chemicalProperties = CellData::chemicalPropertiesOf(CHROMATIN,
                                                                               static_cast<Activity>(chainConfig.isActive()));
        Flags flags = CellData::flagsOf(static_cast<Transcribability>(chainConfig.isTranscribable()),
                                        static_cast<TranscriptionInhibition>(!chainConfig.isInhibited()));
        auto newChain = GridInitializer::initializeGridWithStepInstructions(grid, allChains, column, row,
                                                                            chainConfig.getSteps(),
                                                                            chemicalProperties, flags);
        
        auto pos = chainProperties.find("Cutoff");
        if (pos != chainProperties.end() && pos->second != 0)
        {
            cutoffChains.insert(newChain.begin(), newChain.end());
        }
        
        // Keeping track of permissible chains, so that we can also switch their transcribablility easily.
        if (!chainConfig.isInhibited())
        {
            permissibleChains.insert(newChain.begin(), newChain.end());
        }
    }
    // Chain construction over
    
    
    int numRbpCells = grid.getSpeciesCount(RBP);
    int numChromatinCells = numInnerCells - numRbpCells;
    double rbpRatio = static_cast<double>(numRbpCells) / numInnerCells;
    double chromatinRatio = static_cast<double>(numChromatinCells) / numInnerCells;
    logger.logMsg(PRODUCTION, "GRID: %s=%d, %s=%d, %s=%.3f, %s=%.3f", DUMP(numChromatinCells), DUMP(numRbpCells),
                  DUMP(chromatinRatio), DUMP(rbpRatio));
    logger.logMsg(PRODUCTION, "CHAINS: %s=%d, %s=%d, %s=%d", DUMP(allChains.size()), DUMP(cutoffChains.size()), DUMP(permissibleChains.size()));
    logger.logMsg(PRODUCTION, "Initializing microemulsion: %s=%f, %s=%f, %s=%f, %s=%f, %s=%f, %s=%f, %s=%f, %s=%f",
                  DUMP(dtChem), DUMP(kOn), DUMP(kOff), DUMP(kChromPlus), DUMP(kChromMinus), DUMP(kRnaPlus),
                  DUMP(kRnaMinus), DUMP(kRnaTransfer));
    Microemulsion microemulsion(grid, omega, logger,
                                dtChem, kOn, kOff, kChromPlus, kChromMinus, kRnaPlus, kRnaMinus, kRnaTransfer,
                                stickyBoundary);
    
    // Initialize PgmWriters for the 3 channels
    PgmWriter dnaWriter(logger, columns, rows, outputDir + "/microemulsion_DNA", "DNA",
                        [](const CellData &cellData) -> unsigned char {
                            return (unsigned char) 255 * CellData::isChromatin(cellData.chemicalProperties);
                        });
    PgmWriter rnaWriter(logger, columns, rows, outputDir + "/microemulsion_RNA", "RNA",
                        [](const CellData &cellData) -> unsigned char {
                            RnaCounter rnaContent = cellData.rnaContent;
                            if (rnaContent > 255) // Saturate in a proper way
                            {
//                                logger.logMsg(WARNING, "PgmWriter: SATURATION - RNA value of %d exceeds 255", rnaContent);
                                rnaContent = 255;
                            }
                            //TODO: check if actually we need to avoid to show TXN sites even if they have RNA, it seems
                            //TODO[cont]: that the real data behave in an non-related way for TXN and RNA.
                            return (unsigned char) rnaContent;
                        });
    PgmWriter transcriptionWriter(logger, columns, rows, outputDir + "/microemulsion_Transcription", "Pol II Ser2Phos",
                                  [](const CellData &cellData) -> unsigned char {
                                      return (unsigned char) 255 *
                                             CellData::isActiveChromatin(cellData.chemicalProperties);
                                  });
    dnaWriter.setData(grid.getData());
    rnaWriter.setData(grid.getData());
    transcriptionWriter.setData(grid.getData());
    
    // --- actual iteration steps of the simulation are carried out from here on ...
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
                              allChains, cutoffChains, permissibleChains, kOn, kOff,
                              kChromPlus, kChromMinus, kRnaPlus, kRnaMinus,
                              kRnaTransfer,
                              t/timeMultiplier);
        }
        // Time-stepping loop
        while (t < snapshotSchedule.getNextEventTime())
        {
            t += dt;
//            swapsPerformed += microemulsion.performRandomSwap();
//            ++swapAttempts;
            swapsPerformed += microemulsion.performRandomSwaps(swapRounds);
            swapAttempts += cellsPerColour * swapRounds;
            
            // Now check if to perform chemical reactions
            if (t >= nextChemTime)
            {
                chemChangesPerformed += microemulsion.performChemicalReactions();
                nextChemTime += dtChem;
            }
        }
        
        // Writing the required snapshot(s)
        if (snapshotSchedule.check(t))
        {
            auto eventsToApply = snapshotSchedule.popEventsToApply(t);
            for (auto event : eventsToApply)
            {
                takeSnapshots(logger, dnaWriter, rnaWriter, transcriptionWriter, t/timeMultiplier, swapAttempts,
                              swapsPerformed, chemChangesPerformed, event == GENERIC_EXTRA_SNAPSHOT);
            }
        }
    }
    logger.logEvent(DEBUG, t, "Exiting main time-stepping loop");
    // --- iteration steps of simulation are over here
    
    //
    return 0;
}



void applyCutoffEvents(Logger &logger, EventSchedule<CutoffEvent> &eventSchedule, Microemulsion &microemulsion,
                       const std::set<ChainId> &allChains, const std::set<ChainId> &cutoffChains,
                       const std::set<ChainId> &permissibleChains, double kOn, double kOff, double kChromPlus,
                       double kChromMinus, double kRnaPlus, double kRnaMinus, double kRnaTransfer, double t)
{
    // TODO: we should be using the command pattern for all events...
    auto eventsToApply = eventSchedule.popEventsToApply(t);
    for (auto event : eventsToApply)
    {
        if (event == FLAVOPIRIDOL)
        {
            logger.logEvent(PRODUCTION, t, "EVENT: Applying Flavopiridol condition");
            microemulsion.setKChromPlus(0);
        }
        else if (event == ACTINOMYCIN_D)
        {
            logger.logEvent(PRODUCTION, t, "EVENT: Applying Actinomycin D condition");
            microemulsion.setKChromPlus(0); // Chromatin state no longer changes
            microemulsion.setKChromMinus(0); // Chromatin state no longer changes
            microemulsion.setKRnaPlus(0); // RNA production is halted
            microemulsion.setKRnaMinusTxn(0); // RNA attached at transcription site is not degraded
            microemulsion.setKRnaTransfer(0); // RNA should not be transferred from TXN sites to RBP
        }
        else if (event == ACTIVATE)
        {
            logger.logEvent(PRODUCTION, t, "EVENT: Activating transcription");
            microemulsion.setKOn(kOn);
            microemulsion.setKOff(kOff);
            microemulsion.setKChromPlus(kChromPlus);
            microemulsion.setKChromMinus(kChromMinus);
            microemulsion.setKRnaPlus(kRnaPlus);
            microemulsion.setKRnaMinusRbp(kRnaMinus);
            microemulsion.setKRnaMinusTxn(0);
            microemulsion.setKRnaTransfer(kRnaTransfer);
        }
        else if (event == TXN_SPIKE)
        {
            // NOTE: transcription spike DOES NOT include activation
            logger.logEvent(PRODUCTION, t, "EVENT: Transcription spike");
            microemulsion.enableTranscribabilityOnChains(permissibleChains);
        }
        else
        {
            // Testing playground here...
            logger.logEvent(PRODUCTION, t, "EVENT: Applying custom cutoff conditions");
//            microemulsion.setKRnaMinusRbp(0);
            microemulsion.enablePermissivityOnChains(allChains);
            microemulsion.disablePermissivityOnChains(cutoffChains);
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
