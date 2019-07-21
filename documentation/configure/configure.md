# Configure
To actually run a simulation, we execute the `active-microemulsion` file. This file takes input arguments directly on the command line, and additionally needs to be pointed to a configuration file. The configuration file will tell the executable how to initialize the simulation at the first time point. This includes the configuration of chromatin chains and their different chemical states.

On a fundamental level, the executable will use a turtle-type approach to trace out the chromatin chains. You will not have to interact at the lowest level with this routine. Instead, there is a python script, `chainConfigurator.py` from which you can request the properties of the chain layout you desire. The script will then generate an according chain configuration for you. This configuration can, in turn, be used as an input for the `active-microemulsion` executable.

## Creating configuration files on Mac
To generate chain configuration files, you need to use python 3. On Mac, an older python version is the default, so you should be making sure you run the configuration script using python 3. We found instructions how to control which python version is actually used, based on the `pyenv` tool. The instructions can be found here.
`https://opensource.com/article/19/5/python-3-default-macos`

There's a part where it says you have to add a line to your shell's path. This means you have to go to your home directory `~\` and edit `.bash-profile` to add this line. Next problem, `.bash_profile` does not exist per default on Mac. So you have to go and create it. Use the terminal and `cd ~`. `touch .bash_profile`. `open -a TextEdit.app .bash_profile`.

Then, literally copy and paste `echo -e 'if command -v pyenv 1>/dev/null 2>&1; then\n  eval "$(pyenv init -)"\nfi' >> ~/.bash_profile` to your terminal and hit Enter. Now, as you open a new terminal, `which python` should give some `pyenv` stuff.

A more in-depth explanation of python version management with `pyenv` is given here:
`https://github.com/pyenv/pyenv`

You can check by `pyenv version` which version is currently chose, and you can change it via `pyenv global 3.7.4` (here, please check which version higher than 3 you have installed).

OK, now it goes on - python is missing `numpy`, will it ever end! Sorry...go ahead and install `pip3`, the package manager for python 3. You can do so by `brew install pip3`. Then, you can use `pip3 install numpy`. 

Now, finally, you should be able to execute this example chain generation command:
`python utils/chainConfigurator.py -W 50 -H 50 -s -I 0.4 -n 25 ./test.chains`

You should see a file `test.chains` in the repository directory, and it should contain editor readable instructions for chain generation.

## Creating configuration files on the bwUniCluster
You will also have to load python 3. This is considerably easier, as you simply load the according module. This goes by `module load devel/python/3.5.0`. Then you can use the same example command to test whether you can generate a test chain configuration.

## Left-over old documentation part

For further details on the format, see [Issue #6](https://github.com/lhilbert/active-microemulsion/issues/6#issuecomment-418184217).

Configuration files can be generated with the `utils/chainConfigurator.py` script.

