# Scheduling on BwUniCluster

## Setup
The scripts available in this directory should be either copied or linked to the directory where the 
`active-microemulsion` executable is placed. This is usually in the repository base directory.

## How does it work?
On BwUniCluster we have an upper limit on the number of jobs we can schedule at the same time.
Therefore we want to bundle several simulations into a single job which will be scheduled.
This can be easily achieved by using the `parbatch` tool, developed by the BwUniCluster sysadmins.

In this directory we have 3 scripts:
- `runSimulationSet.sh`  
  This is the main "control panel" where the different simulation parameters can be configured.
  The script will then combine them together and generate the actual parameters for all the runs
  in the control, flavopiridol and actinomycin D cases.  
  The script then bundles 10 of these simulation runs into a single job file which is then passed
  to the parbatch wrapper script. 
- `parbatch-wrapper.sh`  
  This is just a wrapper around parbatch: it loads the correct module and calls it with the right arguments.
- `moab-job.sh`  
  This the actual script that is run on the target machine for each simulation run.
  The job file passed to parbatch contains a call to this script for each of the simulations.
  This script makes sure to load any required module and to call the chain configuration generator script
  so that chains can be configured on the fly right before launching the simulation.
  
## How to schedule?
Once the setup step has been carried out, it is just necessary to:
1. Edit the `runSimulationSet.sh` script to configure the required parameters for the simulations
you need to run.
2. Execute it with: `./runSimulationSet.sh`

## How to manage the scheduled jobs?
- To list the currently scheduled/running jobs, use this command: `showq`
- To cancel a job, use this command: `canceljob <jobID>`
- For anything else and further information, please refer to the official documentation:
    - [General bwHPC documentation about Batch Jobs](https://www.bwhpc-c5.de/wiki/index.php/Batch_Jobs)
    - [Specific BwUniCluster additional features](https://www.bwhpc-c5.de/wiki/index.php/Batch_Jobs_-_bwUniCluster_Features)
