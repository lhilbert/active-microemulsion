# Configure
`active-microemulsion` reads parameters from the arguments passed on the command-line, there is no configuration file to write.
The only configuration which is passed through file is the one defining the initial layout for the chromatin chains.

## Configuring the initial chromatin layout
`active-microemulsion` accepts plain-text files carrying instructions on how to initialize the chromatin chains.
The principle behind this configuration is to "program the turtle", i.e. to have a cursor which movements on the grid draw the chromatin chain and the configuration file carries the list of instructions for drawing each chain.

For further details on the format, see [Issue #6](https://github.com/lhilbert/active-microemulsion/issues/6#issuecomment-418184217).

Configuration files can be generated with the `utils/chainConfigurator.py` script.
