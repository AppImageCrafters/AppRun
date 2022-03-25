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

# libapprun_hooks.so
AppDir/lib/x86_64-linux-gnu/libapprun_hooks.so
```
**_NOTE:_** Library paths can change according to the system architecture.

## Runtimes and libc deployment

To allow running dynamically linked binaries and scrips using the interpreters contained in the bundle we need to deploy
them into two special directories at `$APPDIR/runtime`: `compat` and `default`. The `compat` runtime will contain libc 
binaries and links to the embed interpreters. This will be used when the bundle is executed on a system with an older 
libc. The `default` runtime will contain a symlink to interpreters embed in the bundle including the `ld-linux.so` but
this will be linked to the one in the system root. This runtime will be used when the system have a newer libc than
the bundle.

Example runtimes layout:
```shell
# Compat runtime
AppDir/runtime/compat/lib64/ld-linux-x86-64.so.2
AppDir/runtime/compat/lib/x86_64-linux-gnu/libtinfo.so.6
AppDir/runtime/compat/lib/x86_64-linux-gnu/libc.so.6
AppDir/runtime/compat/lib/x86_64-linux-gnu/libdl.so.2
AppDir/runtime/compat/usr/bin/bash # links to: ../../../../usr/bin/bash

# Default runtime
AppDir/runtime/default/lib64/ld-linux-x86-64.so.2 # links to: /lib64/ld-linux-x86-64.so.2
AppDir/runtime/compat/usr/bin/bash # links to: ../../../../usr/bin/bash
```

## Make interpreters paths relative

### Patch PT_INTERP on dynamically linked executables

To allow AppRun to select the right `ld-linux` at runtime we need to make relative the path stored on the `PT_INTERP` 
tag of all dynamically linked executables. You can use `patchelf` or other similar tool to achieve this.

Example bash instructions:
```shell
TARGET=AppDir/bin/bash
LINKER=$(patchelf --print-interpreter "$TARGET")
patchelf --set-interpreter "${LINKER:1}" "$TARGET" 
```
**_NOTE:_** The `"${LINKER:1}"` expression removes the first character of a string which is `/`.

### Patch Shebang on scripts

As there are no warranties that a given interpreter (i.e.: bash) is present on all target systems you must include
it as part of your bundle in order to allow any script to be executed. Additionally, you need to patch the shebang 
interpreter path to make it relative to the current work dir.

Example bash instructions:
```shell
TARGET=AppDir/bin/script.sh
sed -i 's|#\![[:space:]]*\/*|#\! |' $TARGET
```
**_NOTE:_** The sed expression matches `#! /` and replaces it by `#! `, which makes the interpreter path relative.

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

- `APPDIR_LIBC_VERSION`: version of the libc bundled.
- `APPDIR_LIBC_LINKER_PATH`: `ld-linux.so` relative paths separated by `:`. Example: `lib/ld-linux.so.2:lib64/ld-linux-x86-64.so.2`
- `APPDIR_LIBC_LIBRARY_PATH`: libc library paths contained in the compat runtime separated by `:`.

- `APPDIR_PATH_MAPPINGS`: list of path mappings separated by semicolon, a path mapping is composed by two paths 
separated by a colon. Example: `/usr/lib/myapp:$APPDIR/usr/lib/myapp;`
