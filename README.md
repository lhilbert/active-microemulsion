# active-microemulsion
Package for the lattice-based simulation of active polymer microemulsions, such as DNA.

## How to build
### Dependencies
`active-microemulsion` depends on the `boost-program-options`, `boost-system` and `boost-filesystem` libraries (tested on version 1.62). See [https://www.boost.org/](https://www.boost.org/) for further information on the libraries and on how to install them.

On **Ubuntu** these can easily be installed via the official repositories using the `apt` package manager:
1. Either by installing the `libboost-program-options-dev`, `libboost-system-dev` and `libboost-filesystem-dev` packages.
2. Or by installing the entire `libboost-all-dev` metapackage.

On **macOS** (Sierra Version 10.12.6) we installed the follwing libraries using Homebrew:
```
brew install gcc
brew install boost
```

The `utils/sequenceFigureBuilder.sh` script depends on the *ImageMagick* suite for producing figures, specifically on the `convert` and `montage` commands.
These can be obtained on Ubuntu from the official repositories by installing the `imagemagick` package.

### How to actually build?
Full documentation for how to build, configure and run can be found at the [documentation page](https://github.com/lhilbert/active-microemulsion/tree/master/documentation) of the repository.

You can also find a [tutorial for building and scheduling on BwUniCluster](https://github.com/lhilbert/active-microemulsion/blob/master/documentation/tutorial.md) (work in progress...).

## Usage
Basic usage:
```
./active-microemulsion
```
Getting the help:
```
./active-microemulsion --help
Supported options:
  -h [ --help ]                         Show this help and exit
  -d [ --debug ]                        Enable the debug logging level
  --coarse-debug                        Enable the coarse_debug logging level
  -q [ --quiet ]                        Restrict logging to PRODUCTION,WARNING,
                                        ERROR levels
  -Q [ --Quiet ]                        Restrict logging to WARNING,ERROR 
                                        levels
  -m [ --minutes ]                      Time variables are expressed in minutes
                                        instead of seconds
  --no-chain-integrity                  Do not enforce chain integrity
  --no-sticky-boundary                  Do not make boundary sticky to 
                                        chromatin
  --flavopiridol arg                    Apply Flavopiridol at cutoff time(s). 
                                        Cutoff time(s) can be specified as 
                                        parameter
  --actinomycin-D arg                   Apply Actinomycin D at cutoff time. 
                                        Cutoff time(s) can be specified as 
                                        parameter
  --activate arg                        Activate transcription at cutoff time. 
                                        Cutoff time(s) can be specified as 
                                        parameter
  --txn-spike arg                       Set a transcription spike at cutoff 
                                        time. Cutoff time(s) can be specified 
                                        as parameter
  -o [ --output-dir ] arg (=./Out)      Specify the folder to use for output 
                                        (log and data)
  -i [ --input-image ] arg              Specify the image to be used as initial
                                        value for grid configuration
  -P [ --chains-config ] arg (=testConfig.chains)
                                        Specify the chains config file to be 
                                        used for grid configuration
  -T [ --end-time ] arg (=1000)         End time for the simulation
  -C [ --cutoff-time ] arg (=-1)        Time at which the chemical reaction 
                                        cutoff takes place
  -c [ --cutoff-time-fraction ] arg (=1)
                                        Fraction of endTime at which the 
                                        chemical reaction cutoff takes place
  -t [ --snapshot-interval ] arg (=-1)  Time interval (in seconds) between 
                                        visualization snapshots. A negative 
                                        time lets the '-S' flag take over
  -S [ --number-snapshots ] arg (=100)  Number of visualization snapshots to 
                                        take. Only applied if '-t' is negative 
                                        or not set
  -e [ --extra-snapshot ] arg (=-1)     Time interval (in seconds) between 
                                        cutoff events and their respective 
                                        extra snapshot. A negative time 
                                        disables the extra snapshot.
  -E [ --extra-snapshot-abs ] arg (=-1) Time (in seconds) when the extra 
                                        snapshot should be taken. A negative 
                                        time disables this setting. This 
                                        overrides -e.
  --additional-snapshots arg            Explicitly add additional snapshot 
                                        time(s). Snapshot time(s) can be 
                                        specified as parameter 
                                        (space-separated)
  -W [ --width ] arg (=50)              Width of the simulation grid
  -H [ --height ] arg (=50)             Height of the simulation grid
  --threads arg (=-1)                   Number of threads to use for 
                                        parallelization. A negative value lets 
                                        OMP_NUM_THREADS take precedence
  -w [ --omega ] arg (=0.25)            Energy cost for contiguity of 
                                        non-affine species (omega model 
                                        parameter)
  -s [ --sppps ] arg (=3000)            Number of average swap attempts per 
                                        pixel per second
  --kOn arg (=0.00125)                  Reaction rate - Chromatin from 
                                        non-transcribable to transcribable 
                                        state
  --kOff arg (=0.0025000000000000001)   Reaction rate - Chromatin from 
                                        transcribable to non-transcribable 
                                        state
  --kChromPlus arg (=0.0044444000000000003)
                                        Reaction rate - Transcription turned ON
  --kChromMinus arg (=0.0011111000000000001)
                                        Reaction rate - Transcription turned 
                                        OFF
  --kRnaPlus arg (=0.041666599999999998)
                                        Reaction rate - RBP from free to bound 
                                        state
  --kRnaMinusRbp arg (=0.000416666)     Reaction rate - RBP from bound to free 
                                        state
  --kRnaTransfer arg (=0.016666666659999999)
                                        Reaction rate - RNA migrating from 
                                        transcription site to an RBP site
```
