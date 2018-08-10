# active-microemulsion
Package for the lattice-based simulation of active polymer microemulsions, such as DNA.

## How to build
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
