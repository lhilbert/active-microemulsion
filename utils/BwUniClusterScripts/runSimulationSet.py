#!/usr/bin/env python3

from schedulingLib import SimulationSet

emailAddress = "foo@bar"
notifyEvents = "bea" # b=begin, e=end, a=abort

activation = 10
endTime = 90

simSet = SimulationSet(emailAddress,
                       18,
                       "FooBar",
                       endTime,
                       notifyEvents,
                       (100,100),
                       staticOpts="-w 0.25 -s 4500 --kRnaPlus 1.1111e-1 --kRnaMinusRbp 5.5555e-4 --kChromPlus 0.075 --kChromMinus 1.6666e-3 --kOn 1.852e-4 --kOff 1.6666e-3 --kRnaTransfer 5.5555e-4 -m -t 1",
                       chainGeneratorOpts="--chain-generator-sparse --chain-generator-I 0.4 --chain-generator-n 25",
                       activationTime=activation,
                       dryRun=True,
                       additionalSnapshots=(10.1, 10.2, 10.3, 10.4, 10.5, 10.6, 10.7, 10.8, 10.9, 11.25, 11.5, 11.75, 12.5, 13.5, 14.5, 15.5, 16.5, 17.5, 18.5, 19.5),
                       additionalTreatmentTimes=[x/10 for x in range(101,109,1)]
                                                + [x/10 for x in range(201,209,1)])
simSet.schedule()
print("--> Jobs scheduled: %d", simSet.numBundles())
print("--> Simulations scheduled: %d", simSet.numSimulations())

# emailAddress,
# wallTimeHours,
# simSetName,
# endTime,
# notifyEvents="bea",
# size=(100, 100),
# chainGeneratorOpts="--chain-generator-sparse --chain-generator-I 0.4 --chain-generator-n 25",
# staticOpts="",
# additionalSnapshots=(),
# activationTime=0,
# activationEvents=("--txn-spike", "--activate"),
# treatmentFlags=("--flavopiridol", "--actinomycin-D"),
# additionalTreatmentTimes=(),
# performControl=True,
# performAdditionalRelaxations=True,
# treatment2EndDelay=30,
# dryRun = False

