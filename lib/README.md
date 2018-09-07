# lib
This folder should contain links to custom-built shared library objects.

### Note
Remember to also link the headers of the linked shared libraries into the `./include` folder.  
More info on this on the [include](../include/README.md) documentation page.

## How to
Here, as example, is how to link the Boost libraries, in case you built them in userspace.  
Assuming the target install directory for Boost is `~/Libs/Boost/boost_1_68_0`, then run the following command from within this folder:
```
ln -s ~/Libs/Boost/boost_1_68_0/lib/* ./
```

If you build `active-microemulsion` via CMake, then the libraries will be found automatically by the linker.
