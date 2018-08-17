# active-microemulsion
Package for the lattice-based simulation of active polymer microemulsions, such as DNA.

## How to build
### Dependencies
`active-microemulsion` depends on the `boost-program-options`, `boost-system` and `boost-filesystem` libraries (tested on version 1.62). See [https://www.boost.org/](https://www.boost.org/) for further information on the libraries and on how to install them.

On **Ubuntu** these can easily be installed via the official repositories using the `apt` package manager:
1. Either by installing the `libboost-program-options-dev`, `libboost-system-dev` and `libboost-filesystem-dev` packages.
2. Or by installing the entire `libboost-all-dev` metapackage.

The `experiments.sh` script depends on the *ImageMagick* suite for producing figures, specifically on the `convert` and `montage` commands.
These can be obtained on Ubuntu from the official repositories by installing the `imagemagick` package.

### Actual build
Both make and cmake are supported. Building with make just requires:  
```
make
```

## Usage
Basic usage:
```
./active-microemulsion
```
Getting the help:
```
./active-microemulsion --help                                                                                                                                                 [c344dc3]
Supported options:
  -h [ --help ]                    Show this help and exit
  -d [ --debug ]                   Enable the debug logging level
  -q [ --quiet ]                   Restrict logging to PRODUCTION,WARNING,ERROR
                                   levels
  -o [ --output-dir ] arg (=./Out) Specify the folder to use for output (log 
                                   and data)
  -i [ --input-image ] arg         Specify the image to be used as initial 
                                   value for grid configuration
  -T [ --end-time ] arg (=1000)    End time for the simulation
```
