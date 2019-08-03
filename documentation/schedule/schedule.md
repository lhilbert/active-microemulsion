# Schedule on cluster
## BwUniCluster

After we have downloaded the repository and built an executable ([Build an executable program]), we still have to copy some utility scripts into place. Specifically, we have to copy from the directory ```utils/bwUniClusterScripts``` the files ```runSimulations.py```, ```schedulingLib.py```, ```parbatch-wrapper.sh```, and ```moab-job.sh```. Then, for. every. time we log in to the cluster access node, we have to ```module load devel/python/3.5.0```, so that we can execute the python scripts.

Now, to actually schedule a set of simulations as required for the microemulsion project, edit the ```runSimulations.py``` folder to enter your parameters, length of simulation etc.

Then, you can schedule the simulations using the command ```./runSimulations.py```. To see which jobs are currently running, use the command ```showq```. To cancel a job, use ```canceljob <jobID>```.

## Outdated documentation
The outdated documentation about how to schedule on BwUniCluster is available [here](../../utils/BwUniClusterScripts/README.md) (`utils/BwUniClusterScripts`).

The examples for the scripts mentioned in the above page are available in the repository, under `utils/BwUniClusterScripts/`.
