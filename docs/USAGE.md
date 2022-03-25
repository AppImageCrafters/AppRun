# AppRun Framework Usage

Instructions to create a portable bundle (AppDir) using the **AppRun Framework**.

## Project layout

To create a portable bundle first you have to deploy `AppRun`, `libapprun_hooks.so` and your application binaries along
with its dependencies into a directory with the following layout:

```shell
# Your app binaries and resources
AppDir/usr/bin/app
AppDir/usr/lib/*
AppDir/usr/share/*

# AppRun
AppDir/AppRun
AppDir/AppRun.env

# hooks lib
AppDir/lib/x86_64-linux-gnu/libapprun_hooks.so
```
**_NOTE:_** Library paths can change according to the system architecture.

## Libc deployment

The libc and ld-linux binaries will be deployed using a different prefix to allow excluding it at runtime if the system
has a newer version. Usually `$APPDIR/opt/libc/` is used as prefix.

```shell
AppDir/opt/libc/lib64/ld-linux-x86-64.so.2
AppDir/opt/libc/lib64/lib/x86_64-linux-gnu/libtinfo.so.6
AppDir/opt/libc/lib64/lib/x86_64-linux-gnu/libc.so.6
AppDir/opt/libc/lib64/lib/x86_64-linux-gnu/libdl.so.2
``` 

## Linker path setup on dynamically linked executables

To allow AppRun to select the right `ld-linux` at runtime we need to make relative the path stored on the `PT_INTERP` 
tag of all dynamically linked executables. You can use `patchelf` or other similar tool to achieve this.

Here is an example of how to do it in bash:
```shell
TARGET=AppDir/bin/bash
LINKER=$(patchelf --print-interpreter "$TARGET")
patchelf --set-interpreter "${LINKER:1}" "$TARGET" 
```

**_NOTE:_** The `"${LINKER:1}"` expression removes the first character of a string which is `/`.

## Environment configuration

Once we have all the binaries in place we proceed to create the `AppRun.env` file. This file must be placed next
to the `AppRun` binary and must be used to define the environment variables required by the bundled application to
run properly. It supports using environment variables using the following notation `$VARNAME` also support
the special bash variable `$@` to forward the execution arguments.

### AppRun Special Entries

The following entries are required by AppRun to configure the runtime environment variables of the target application 
and decide which `ld-linux.so` and `libc.so` will be used. 

- `APPDIR`: Points the bundle root dir.
- `APPDIR_EXEC_PATH`: Absolute path to the main application executable. Example: `$APPDIR/usr/bin/app` 
- `APPDIR_EXEC_ARGS`: Arguments to be passed to the application. Example: `$@`

- `APPDIR_LIBRARY_PATH`: application library dir paths separated by `:`. Must contain the dir path where 
`libapprun_hooks.so` was deployed. Example: `$APPDIR/lib:$APPDIR/usr/lib`

- `APPDIR_LIBC_PREFIX`: prefix in which the `ld-linux.so` binary was deployed. Example: `$APPDIR/opt/libc`
- `APPDIR_LIBC_VERSION`: version of the libc bundled.
- `APPDIR_LIBC_LINKER_PATH`: `ld-linux.so` relative paths separated by `:`. Example: `lib/ld-linux.so.2:lib64/ld-linux-x86-64.so.2`
- `APPDIR_LIBC_LIBRARY_PATH`: libc library paths contained in the compat runtime separated by `:`.

- `APPDIR_PATH_MAPPINGS`: list of path mappings separated by semicolon, a path mapping is composed by two paths 
separated by a colon. Example: `/usr/lib/myapp:$APPDIR/usr/lib/myapp;`

