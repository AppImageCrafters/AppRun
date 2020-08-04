# AppRun

This project holds a collection of tools to properly setup and control the AppImage (and AppDir) execution environment.

## AppRun

AppRun is an entry point for AppImages which carry a copy of glib and ld-linux. It allows to chose at runtime the
newer glibc/ld-linux binary to use so every library can find all the required symbols. To do so it read the GLIBC_VERSION
from the system libc binary and compares it with the one bundled choosing the newer one.

Additionally it setups the execution environment for the bundled application in a way that it can be cleaned up by
`libapprun_hooks` on forks.  
 
AppRun requires `.env` file next to it, with the following variables: 
- APPIMAGE_UUID: unique identifier of the AppImage binary
- SYSTEM_INTERP: path to the system interpreter. Original value of the binaries PT_INTERP
- APPDIR_INTERP: path to the bundle interpreter.
- RUNTIME_INTERP: path to the interpreter to be used at runtime. Rationale, ELF files PT_INTERP value cannot be changed
at runtime therefore AppRun makes a symlink in this path to the correct interpreter binary.

- EXEC_PATH: path to the application binary
- EXEC_ARGS: arguments to pass when executing the application binary, BASH like variables are allowed.

- APPDIR_LIBRARY_PATH: paths to the bundle library dirs
- LIBC_LIBRARY_PATH: paths to the bundle glibc related library dirs. Only those libraries that are bundled to an 
specific version of glibc should be placed here. This path *MUST* not overlap with APPDIR_LIBRARY_PATH otherwise
it will not be possible to separate effectively the libaries.
- APPDIR_LIBC_VERSION: bundled libc version. Just te save us parsing the bundled libc file on every execution
- LD_PRELOAD: libapprun_hooks binary name.

**NOTE**: Other variables added to `.env` file will be set as part of the runtime environment.  

Example `.env` file:
```
APPIMAGE_UUID=955d9b0b-1310-4c0e-9fa0-9ccf77cafdef

SYSTEM_INTERP=/lib64/ld-linux-x86-64.so.2
APPDIR_INTERP=$APPDIR/opt/libc/lib/x86_64-linux-gnu/ld-2.27.so
RUNTIME_INTERP=/tmp/appimage-955d9b0b-1310-4c0e-9fa0-9ccf77cafdef-ld-linux.so.2

EXEC_PATH=$APPDIR/usr/bin/obs
EXEC_ARGS=$@

APPDIR_LIBRARY_PATH=$APPDIR/usr/lib:$APPDIR/lib/x86_64-linux-gnu:$APPDIR/usr/lib/x86_64-linux-gnu:$APPDIR/usr/lib/x86_64-linux-gnu/pulseaudio
LIBC_LIBRARY_PATH=$APPDIR/opt/libc/usr/lib/x86_64-linux-gnu:$APPDIR/opt/libc/lib/x86_64-linux-gnu:$APPDIR/opt/libc/usr/local/lib/x86_64-linux-gnu
APPDIR_LIBC_VERSION=2.27
LD_PRELOAD=libapprun_hooks.so
``` 

## libapprun_hooks 
This library is intended to be used together with the AppImage distribution mechanism.
Place the library somewhere in your AppDir and point `LD_PRELOAD` to it before launching your application.

Whenever your application invokes a child process through execv() or execve(),
this wrapper will intercept the call and properly setup the target execution environment.

If the target binary is located INSIDE the AppImage/AppDir bundle:
- the AppImage private environment is keep 
- the *INTERPRETER* is prepended to the exec instruction   

If the target binary is located OUTSIDE the AppImage/AppDir bundle:
- the environment is restored to the state before launching the AppImage keeping any changes performed by the 
application.

The following environment variables are required:
- `$INTERPRETER`: must contain the full path of the interpreter (ld-linux.so.2) to be use while executing bundled binaries.
- `$APPDIR`: path of the AppDir you are launching your application from.

**Private environment variables are defined as follows**:  

Where {VAR} is the name of the environment variable (e.g. "PATH"):
- `$APPRUN_ORIGINAL_{VAR}`: original value of the environment variable. Will be restore when executing external binaries.
- `$APPRUN_STARTUP_{VAR}`: value of the variable when you were starting up your application. Used as reference to 
detect changes made by the application.

**Example:**

```shell script
export APPRUN_ORIGINAL_PATH=$PATH
export PATH=$APPDIR/bin:$APPDIR/usr/bin:$PATH
export APPRUN_STARTUP_PATH=$PATH
``` 

Notice that the order matters in order to properly define the ORIGINAL value.                             