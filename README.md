# AppRun

This project provides a framework to create portable application bundles to be used on GNU/Linux systems. It relies on 
a set of hooks over the libc functions and relative paths on the dynamically linked executables.

**_NOTE:_** If you're planing to pack your application as an AppImage please use [appimage-builder](https://appimage-builder.readthedocs.io/). 

## Motivation

To make an application bundle that could be used on different GNU/Linux systems the packager must face different
challenges which include:
- absence of forward compatibility in libraries loaded from the system (libc),
- absolute paths in scripts shebang,
- application specific environment variables, 
- and fixed paths in binaries.

We aim to provide an effective solution for those problems.

## Features

- Creation of forward and backward compatible bundles by configuring the libc and ld-linux to be used at startup.
- Private environment variables setup.
- Path mappings (override fixed paths in binaries).

## Limitations

- Don't work on statically linked executable (those are portable anyway).
- Execution flow should not leave the bundle to ensure the private environment is preserved.


## Roadmap

- Support libstdc++ runtime switching
- Implement hooks on `execvl` functions

## Build Instructions

```shell
git clone https://github.com/AppImageCrafters/AppRun.git
mkdir -p AppRun/build

cd AppRun/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Useful links

- [Usage instructions](docs/USAGE.md)
- libc switching at runtime
- private environment variables
- path mappings
