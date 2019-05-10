### schedulingLib
# Utils and classes for the runSimulationSet.py script
#

import subprocess  # Required by ShellCommand
import os  # Required for creating the directory where to store jobs, if not present

THREADS_PER_BUNDLE = 16


class ShellCommand:
    def __init__(self, cmd, echo=True, dryRun=False, timeoutSec=1):
        if echo:
            print(cmd)

        if dryRun:
            self.o = "#DryRun#"
            self.e = ""
            self.code = 0
            return

        if type(cmd) != list:
            cmd = str(cmd).split(" ")

        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        self.o, self.e = proc.communicate(timeout=timeoutSec)
        self.code = proc.returncode

    def stdout(self):
        return self.o.decode('ascii').strip('\n')

    def stderr(self):
        return self.e.decode('ascii').strip('\n')

    def retcode(self):
        return self.code


class Simulation:
    def __init__(self,
                 folder,
                 numThreads=1,
                 size=(100, 100),
                 chainGeneratorOpts="--chain-generator-sparse --chain-generator-I 0.4 --chain-generator-n 25",
                 staticOpts="",
                 additionalSnapshots=(),
                 eventsOpts="--flavopiridol 0",
                 endTime=1,
                 extraSnapshot=False,
                 executable="./moab-job.sh"):
        if additionalSnapshots is None:
            additionalSnapshots = []
        self.executable = executable
        self.folder = folder
        self.numThreads = numThreads
        self.size = size
        self.chainGeneratorOpts = chainGeneratorOpts
        self.staticOpts = staticOpts
        self.additionalSnapshots = " ".join([str(x) for x in additionalSnapshots])
        self.eventsOpts = eventsOpts
        self.endTime = endTime
        self.extraSnapshot = extraSnapshot

    def __str__(self):
        if type(self.endTime) == float:
            return "hostname; %s --job-folder-name %s --threads %d -W %d -H %d " \
                    "%s %s --additional-snapshots %s " \
                    "%s -E %f -T %f" \
                    % (self.executable, self.folder, self.numThreads, self.size[0], self.size[1],
                       self.chainGeneratorOpts, self.staticOpts, self.additionalSnapshots,
                       self.eventsOpts, self.endTime, self.endTime)
        else:
            return "hostname; %s --job-folder-name %s --threads %d -W %d -H %d " \
                   "%s %s --additional-snapshots %s " \
                   "%s -E %d -T %d" \
                   % (self.executable, self.folder, self.numThreads, self.size[0], self.size[1],
                      self.chainGeneratorOpts, self.staticOpts, self.additionalSnapshots,
                      self.eventsOpts, self.endTime, self.endTime)

    def getEndTime(self):
        return self.endTime

    def __lt__(self, other):
        return self.endTime < other.endTime

    def __gt__(self, other):
        return self.endTime > other.endTime

    def __eq__(self, other):
        return str(self) == str(other)


class ParbatchBundle:
    def __init__(self,
                 emailAddress,
                 wallTimeHours,
                 simulationList,
                 jobNickname="active-simulation",
                 notifyEvents="bea",
                 queue="singlenode",
                 pmem="1024mb",
                 jobFilesFolder="Jobs",
                 submitCmd="msub",
                 parbatchWrapperExecutable="parbatch-wrapper.sh"):
        self.emailAddress = emailAddress
        self.wallTimeHours = wallTimeHours
        self.simulations = simulationList
        self.jobNickname = jobNickname
        self.notifyEvents = notifyEvents
        self.queue = queue
        self.pmem = pmem
        self.nodes = 1
        self.ppn = len(self.simulations)
        self.submitCmd = submitCmd
        self.parbatchWrapperExecutable = parbatchWrapperExecutable
        sysDate = ShellCommand(['date', '+%s.%N'], echo=False)
        # Make sure the Jobs dir is there and then set the jobFileName path
        if not os.path.exists(jobFilesFolder):
            os.makedirs(jobFilesFolder)
        self.jobFileName = "%s/joblist.%s.txt" % (jobFilesFolder, sysDate.stdout())

    def writeJobFile(self):
        with open(self.jobFileName, 'w', encoding='utf-8') as f:
            f.writelines([str(x) for x in self.simulations])
        os.chmod(self.jobFileName, 0o755)

    def dumpJobFileContent(self):
        for s in self.simulations:
            print(s)

    def __str__(self):
        return "%s " \
               "-m %s -M %s " \
               "-q %s -N %s " \
               "-l pmem=%s,nodes=%d:ppn=%d,walltime=%d:00:00 " \
               "-v SCRIPT_FLAGS=%s " \
               "%s" \
               % (self.submitCmd,
                  self.notifyEvents, self.emailAddress,
                  self.queue, self.jobNickname,
                  self.pmem, self.nodes, self.ppn, self.wallTimeHours,
                  self.jobFileName,
                  self.parbatchWrapperExecutable)


class SimulationSet:
    def __init__(self,
                 emailAddress,
                 wallTimeHours,
                 simSetName,
                 endTime,
                 notifyEvents="bea",
                 size=(100, 100),
                 chainGeneratorOpts="--chain-generator-sparse --chain-generator-I 0.4 --chain-generator-n 25",
                 staticOpts="",
                 additionalSnapshots=(),
                 activationTime=0,
                 activationEvents=("--txn-spike", "--activate"),
                 treatmentFlags=("--flavopiridol", "--actinomycin-D"),
                 additionalTreatmentTimes=(),
                 performControl=True,
                 performAdditionalRelaxations=True,
                 treatment2EndDelay=30,
                 dryRun=False):
        self.emailAddress = emailAddress
        self.wallTimeHours = wallTimeHours
        self.simSetName = simSetName
        self.endTime = endTime
        self.notifyEvents = notifyEvents
        self.size = size
        self.chainGeneratorOpts = chainGeneratorOpts
        self.staticOpts = staticOpts
        self.additionalSnapshots = additionalSnapshots
        self.activationTime = activationTime
        self.activationEvents = activationEvents
        self.treatmentFlags = treatmentFlags
        self.additionalTreatmentTimes = list(additionalTreatmentTimes) if additionalTreatmentTimes else []
        self.control = performControl
        self.relaxations = performAdditionalRelaxations
        self.treatment2EndDelay = treatment2EndDelay
        self.dryRun = dryRun
        self.simulations = []
        self.bundles = []
        #
        self.__generateSimulations()
        self.__bundleSimulations()

    def schedule(self):
        for bundle in self.bundles:
            bundle.writeJobFile()
            cmd = ShellCommand(bundle, dryRun=self.dryRun, timeoutSec=10)
            bundle.dumpJobFileContent()

    def numSimulations(self):
        return len(self.simulations)

    def numBundles(self):
        return len(self.bundles)

    def __generateSimulations(self):
        for endTime in \
                [x for x in range(1, self.endTime + 1)] \
                + [x + self.treatment2EndDelay for x in self.additionalTreatmentTimes]:
            # For each possible endTime, check what runs we can schedule with it

            folder = "SimSet_" + self.simSetName

            # Generate control runs
            self.__generateControlRuns(endTime, folder)

            # Generate additional relaxation runs
            self.__generateRelaxationRuns(endTime, folder)

            # Generate treatment runs
            self.__generateTreatmentRuns(endTime, folder)

    def __bundleSimulations(self):
        # First sort simulations (by endTime) so the bundles have balanced threads
        self.simulations.sort()
        # Now take slices of the simulations list and bundle them
        for i in range(0, len(self.simulations), THREADS_PER_BUNDLE):
            I = min(i + THREADS_PER_BUNDLE, len(self.simulations))
            s = self.simulations[i:I]
            bundle = ParbatchBundle(self.emailAddress,
                                    self.wallTimeHours,
                                    s,
                                    "active-microemulsion-T%d" % (s[-1].getEndTime()))
            self.bundles.append(bundle)

    def __generateTreatmentRuns(self, endTime, folder):
        if endTime > self.activationTime + self.treatment2EndDelay:
            treatmentTime = endTime - self.treatment2EndDelay
            activationFlags = self.getActivationFlags(treatmentTime)
            for treatment in self.treatmentFlags:
                if treatment:
                    if type(treatmentTime) == float:
                        treatmentFlag = treatment + " %f" % (treatmentTime)
                        endTime = float(endTime)
                    else:
                        treatmentFlag = treatment + " %d" % (treatmentTime)
                else:
                    treatmentFlag = ""
                eventsFlags = "--flavopiridol 0 %s %s" % (activationFlags, treatmentFlag)
                curSim = Simulation(folder, 1, self.size,
                                    self.chainGeneratorOpts, self.staticOpts,
                                    self.additionalSnapshots,
                                    eventsFlags,
                                    endTime, True)
                self.simulations.append(curSim)

    def __generateRelaxationRuns(self, endTime, folder):
        if endTime > self.activationTime and endTime <= self.treatment2EndDelay:
            eventsFlags = "--flavopiridol 0"
            curSim = Simulation(folder, 1, self.size,
                                self.chainGeneratorOpts, self.staticOpts,
                                self.additionalSnapshots,
                                eventsFlags,
                                endTime, True)
            self.simulations.append(curSim)

    def __generateControlRuns(self, endTime, folder):
        activationFlags = self.getActivationFlags(endTime)
        eventsFlags = "--flavopiridol 0 %s" % (activationFlags)
        curSim = Simulation(folder, 1, self.size,
                            self.chainGeneratorOpts, self.staticOpts,
                            self.additionalSnapshots,
                            eventsFlags,
                            endTime, True)
        self.simulations.append(curSim)

    def getActivationFlags(self, time):
        activationFlags = ""
        if time > self.activationTime:
            for ae in self.activationEvents:
                activationFlags += "%s %d " % (ae, self.activationTime)
        return activationFlags
