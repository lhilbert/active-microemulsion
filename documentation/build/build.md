# Build

## Cloning the repository
The first step of the build process is to clone this repository. This can be done with the `git clone <repo_address>` command.
E.g. if the repo is to be cloned over https, the command to run is:
```
git clone https://github.com/lhilbert/active-microemulsion.git
```
This will create a folder called `active-microemulsion` inside the current working directory of your terminal.

## Building on bwUniCluster
On bwUniCluster (the HPC cluster for Baden-Wuerttemberg's universities), the build process is straightforward, as all the dependencies are available as modules.

Assuming the repo has been cloned into the `active-microemulsion` folder, it is sufficient to run:
```
cd active-microemulsion
./cmake-with-icc
```
The `cmake-with-icc` script is simply a wrapper which loads the relevant modules for the dependencies and then launches the compilation process using `cmake`.

The process can also be compiled with `gcc` or `clang`, either by using the respective scripts or by manually loading the relevant modules and executing the appropriate cmake commands.

There is more info on building with optimization on the bwUniCluster in the tutorial: https://github.com/lhilbert/active-microemulsion/blob/master/documentation/tutorial.md

## Building on other HPC clusters
Building on other clusters should require simply to identify which modules are responsible for the dependencies. Then simply edit the  `cmake-with-icc` script to load the correct modules.

## Building on workstation
The same scripts can be used for building on personal workstations, as modules are not loaded if the module command is not available.
In this case you need to make sure all the dependencies are installed. For the `active-microemulsion` executable, having Boost is the only requirement. On Ubuntu it can be easily installed with the `libboost-all-dev` package.
