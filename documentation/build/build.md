# Build

To run the simulations on your local machine as well as on HPC clusters, you will have to compile on these machines. To compile means, you take the code from this repository and make it into an executable file that runs the actual simulations. Below are the instructions how to do so. In all cases, you need to start by cloning the repository from here to the machien where you would like to buiid. After this, the process differs.

After you cloned the repository to the local machine, you need to compile the executable from this. Below we provide instructions how to do so on different types of machines.

## Cloning the repository
The first step of the build process is to clone this repository. This can be done with the `git clone <repo_address>` command.
E.g. if the repo is to be cloned over https, the command to run is:
```
git clone https://github.com/lhilbert/active-microemulsion.git
```
This will create a folder called `active-microemulsion` inside the current working directory of your terminal.

If you do not have 'git' installed, you need to do so for cloning. How you do this differs on all platforms. In the case of a local MacBook, you can install github Desktop, and the git command for the command line should then be available. On HPC systems, it is usually available by default.

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

## Building on Mac / OS X
(This is tested on Mojave on MacBook Pro)

You will have to install some dependendencies before building. If you have not done so yet, install homebrew. The instructions for doing so can be found at https://brew.sh

When the homebrew install requests a password, this is your Mac login password.

Then, you need to install the boost libraries, using homebrew:
```
brew install boost
```

Then, you need to install an OpenMP library:
```
brew install libomp
```

Then, you need to navigate into the 'active-microemulsion' directory that you cloned by using 'git clone' (see above). You an use the 'cd' command to get there. Once you navigated into that directory, you can build the active-microemulsion executable by running
```
make
```

You can test the compiled executable by running
```
./active-microemulsion --chains-config ./ChainConfigs/halfActive_50x50.chains -T 100
```

It should produce a new folder "/Out" inside of 'active-micromulsion', which should contain `.pgm` image files. These you can inspect with Preview or by pressing the Space Bar. They should show DNA, RNA, and Transcription channels. Especially the DNA images should display chain- or polymer-like structures.
