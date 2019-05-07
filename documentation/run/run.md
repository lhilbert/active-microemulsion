# Run
Running `active-microemulsion` requires:
- having a chain configuration file providing the initial layout of chromatin (see [here](https://github.com/lhilbert/active-microemulsion/blob/master/documentation/configure/configure.md))
- passing the relevant parameters as command line arguments

The command therefore looks like:
```
./active-microemulsion --chains-config config.chains [OTHER_OPTIONS]
```

The complete list and documentation of all the supported flags is available by running `./active-microemulsion --help`.

For a real-world example of how to launch the executable, please have a look at the scheduling script in the repository.
