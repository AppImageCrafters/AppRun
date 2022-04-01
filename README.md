# AppRun

**A framework to create portable application bundles to be used on GNU/Linux systems**

**_NOTE:_** If you're planing to pack your application as an AppImage please use [appimage-builder](https://appimage-builder.readthedocs.io/). 

## Motivation

To make an application bundle that could be used on different GNU/Linux systems the packager must face different
challenges which include:
- absence of forward compatibility in libraries loaded from the system (i.e.: libc),
- absolute interpreter paths in scripts shebang and elf executables,
- application specific environment variables, 
- and fixed paths in binaries.

We aim to provide an effective solution for those problems.

## Features

- Forward and backward compatible bundles.
- Private environment variables setup.
- Private interpreters.
- Path mappings (override paths in binaries).

## Todo

- Support libstdc++ runtime switching

## Limitations

- Don't work on statically linked executable (those are portable anyway).
- Execution flow should not leave the bundle to ensure the private environment is preserved.
- Path mappings only work on a hooked libc functions

## Setup

```shell
git clone https://github.com/AppImageCrafters/AppRun.git
mkdir -p AppRun/build

cd AppRun/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Useful links

- [Usage Instructions](docs/USAGE.md)
- libc switching at runtime
- private environment variables
- path mappings
- [License](LICENSE)