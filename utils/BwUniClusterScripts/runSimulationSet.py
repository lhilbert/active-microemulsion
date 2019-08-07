#!/usr/bin/env python3

from schedulingLib import SimulationSet

emailAddress = "foo@bar"
notifyEvents = "bea" # b=begin, e=end, a=abort

activation = 600
endTime = 7200

dryRun = True # Switch to False to actually schedule jobs

simSet = SimulationSet(emailAddress,
                       48,
                       "OutDirectoryName",
                       endTime, 
                       notifyEvents,
                       (100,100),
                       staticOpts="-w 0.33 -s 10500 --kRnaPlus 6e-2 --kRnaMinusRbp 5.5555e-4 --kChromPlus 1.5e-3 --kChromMinus 1.6666e-3 --kOn 2.500e-4 --kOff 1.6666e-3 --kRnaTransfer 3.0e-1 -t 60",
                       chainGeneratorOpts="--chain-generator-sparse --chain-generator-I 0.2 --chain-generator-n 25",
                       activationTime=activation,
                       dryRun=dryRun,
                       additionalSnapshots=(),
                       additionalTreatmentTimes=[])
simSet.schedule()
print("--> Jobs scheduled: %d", simSet.numBundles())
print("--> Simulations scheduled: %d", simSet.numSimulations())

# SimulationSet(
#     emailAddress,
#     wallTimeHours,
#     simSetName,
#     endTime,
#     notifyEvents="bea",
#     size=(100, 100),
#     chainGeneratorOpts="--chain-generator-sparse --chain-generator-I 0.4 --chain-generator-n 25",
#     staticOpts="",
#     additionalSnapshots=(),
#     activationTime=0,
#     activationEvents=("--txn-spike", "--activate"),
#     treatmentFlags=("--flavopiridol", "--actinomycin-D"),
#     additionalTreatmentTimes=(),
#     performControl=True,
#     performAdditionalRelaxations=True,
#     treatment2EndDelay=30,
#     repoBaseDir="", # If not set, the CWD will be taken
#     dryRun=False
# )
