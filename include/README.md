# include
This folder should contain links to headers for libraries to include which are not available system-wide.

### Note
If headers for shared libraries are included in this way, remember to also link shared library objects (`*.so` files) into the `./lib` folder.  
More info on this on the [lib](../lib/README.md) documentation page.

## How to
Here, as example, is how to make headers for the Boost libraries available, in case you built them in userspace.  
Assuming the target install directory for Boost is `~/Libs/Boost/boost_1_68_0`, then run the following command from within this folder:
```
ln -s ~/Libs/Boost/boost_1_68_0/include/* ./
```

If you build `active-microemulsion` via CMake, then the headers will be found automatically by the compiler.
