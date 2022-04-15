# AppRun Framework Usage

Instructions to create a portable bundle (AppDir) using the **AppRun Framework**. 

## Project layout

To create a portable bundle first you have to deploy `AppRun`, `libapprun_hooks.so` and your application binaries along
with its dependencies. Application binaries and resources will be deployed using the AppDir as prefix keeping the same
system layout. The `AppRun` executable will be deployed to the `AppDir` root. The `libapprun_hooks.so` library will
be deployed next to the application libraries with the same architecture. 

```shell
# Your app binaries and resources
AppDir/usr/bin/app
AppDir/usr/lib/x86_64-linux-gnu/ (libraries)
AppDir/usr/share (resources)

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
run properly. Variables declared in this file will be set at startup and will be private to the bundle. 

### Specification

All text MUST be UTF-8 encoded, without BOM. Implementations SHOULD reject files that contain invalid UTF-8 data.

Leading whitespace is ignored for all lines, including commands, blank lines and comments.

Any blank line (possibly containing only whitespace) MUST be ignored.

Any line starting with a hash/pound (“#”) is considered a comment for human readers and is ignored.

Each variable is declared using the following pattern:

`VAR=VAL`

Whitespace on each side of the “=” character SHALL be ignored. Trailing whitespace SHALL be ignored.

A backslash (“\”) as the last character of a variable declaration line indicates that the value continues on the 
following line. Implementations MUST join the lines without the line endings and treat the backslash character as a 
single space.

The file MUST NOT contain any lines that are not a variable assignment, not a comment and not blank. Implementations 
MAY reject files containing non-blank, non-comment lines that do not match a variable declaration.

Values may reference other environment variables as follows: `$VAR`.  Example: `PATH=$APPDIR/bin:$PATH`.

The special bash variable `$@` can be also used as part of the value.

### Required Environment Entries

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

### Optional Environment Entries

- `APPDIR_PATH_MAPPINGS`: list of path mappings separated by semicolon, a path mapping is composed by two paths 
separated by a colon. Example: `/usr/lib/myapp:$APPDIR/usr/lib/myapp;`
- `APPDIR_FORCE_BUNDLE_RUNTIME`: By default when executing a program, `libapprun_hooks.so` tries to determine whether the program is part of the AppImage, and if the program isn't, it will reset the various `LD_*` variables so the program will use the host system's libraries. If this environmental variable is set to anon-zero-length string, `libapprun_hooks.so` will assume the program is part of the AppImage and use the AppImage's libraries. This is useful if you're generating executable at runtime that aren't stored in the AppImage's mount point.

### Recommended Environment Entries

Besides, the required environment variables you should define those that allow the bundle binaries to resolve their 
resources at runtime. Here you will find some recommendations:

- `PATH`: executables search path, it's recommended to extend the system definition with the paths to the bundle 
binaries dirs.
- `XDG_DATA_DIR`: data dirs, it's recommend to extend the system definition with `$APPDIR/usr/share` and `$APPDIR/usr/local/share`
