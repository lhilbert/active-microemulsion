# Tutorial
## A step-by-step guide to get simulations to work, from scratch

This guide assumes you are working on BwUniCluster. You should be able to easily adapt it to any other similar computing cluster.

### Cloning the repository
Ssh into BwUniCluster. To clone the repo directly into your home folder, just run:
```
git clone https://github.com/lhilbert/active-microemulsion.git
```
This will create an `active-microemulsion` folder containing the entire repository.

### Building
Go into the repo folder:
```
cd active-microemulsion
```
Now you can do a simple build with:
```
./cmake-with-icc.sh
```
This command will load the right dependency modules and perform the build with the Intel icpc compiler with normal optimization level.

In case you get strange errors (e.g. related to missing OpenMP), just try removing the `cmake-build-icc/` folder and building again.

#### PGO - Profile-Guided Optimization
The Intel compiler supports the so called "Profile-Guided Optimization" or [PGO](https://software.intel.com/en-us/articles/step-by-step-optimizing-with-intel-c-compiler#s5).
This build mode is based on first recording a profile of the application, e.g. from a short run, and then using it for optimizing a subsequent build.
In our case using PGO improves the runtime and is therefore recommended to build using PGO.

To build with PGO follow these steps:
1. Edit `cmake-with-icc.sh` (e.g. using `vim` or `nano` on the cluster) by setting the `BUILD_TYPE` variable to the `PGO` value (you can uncomment and comment the `PGO` and `Release` values respectively).
2. Build with `./cmake-with-icc.sh`. This will create a special version of the executable which is capable of recording the profiling data.
3. Now run a short simulation locally on the login node (you may need to copy the `libiomp5.so` lib locally, the command is `ln -s /opt/intel/compilers_and_libraries/linux/lib/intel64/libiomp5.so /path/to/repository/main/folder/`). It is important to run this simulation with some chain configuration, as e.g. `ChainConfigs/halfActive_*`.
4. Now edit `cmake-with-icc.sh` again and revert the `BUILD_TYPE` to `Release` **and set `ENABLE_PGO_USE=1`**.
5. Finally build again with `./cmake-with-icc.sh`. The `active-microemulsion` executable is now optimized and ready to be used.

### Scheduling on cluster
Prepare the environment by linking some scripts into the main repository directory:
```
./utils/BwUniClusterScripts/initializeEnvironment.sh
```
Then load python3 (this has to be done every time you log-in):
```
module load devel/python/3.5.2
```
Finally the jobs can be configured by editing `./runSimulationSet.py`.
This script has a `dryRun` flag, when set to `True` the script will just print the jobs that it would schedule.
Once you are sure you actually want to schedule the jobs on the cluster, just switch the `dryRun` flag to `True` and then run:
```
./runSimulationSet.py
```
  
Jobs on the cluster can be monitored with
```
showq
```
and, as an emergency button, all the jobs can be cancelled with
```
canceljob ALL
```
