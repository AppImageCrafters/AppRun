# AppRun

This project holds a collection of tools to properly setup and control the AppImage (and AppDir) execution environment.

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
- `$APPIMAGE_ORIGINAL_{VAR}`: original value of the environment variable. Will be restore when executing external binaries.
- `$APPIMAGE_STARTUP_{VAR}`: value of the variable when you were starting up your application. Used as reference to 
detect changes made by the application.

**Example:**

```shell script
export APPIMAGE_ORIGINAL_PATH=$PATH
export PATH=$APPDIR/bin:$APPDIR/usr/bin:$PATH
export APPIMAGE_STARTUP=$PATH
``` 

Notice that the order matters in order to properly define the ORIGINAL value.                             