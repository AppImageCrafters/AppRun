# AppImage Execution Wrapper

This library is intended to be used together with the AppImage distribution mechanism.
Place the library somewhere in your AppDir and point LD_PRELOAD to it before launching your application.

Whenever your application invokes a child process through execv() or execve(),
this wrapper will intercept the call and see if the child process lies
outside of the bundled appdir. If it does, the wrapper will attempt to undo
any changes done to environment variables before launching the process,
since you probably did not intend to launch it with e.g. the LD_LIBRARY_PATH
you previously set for your application.

To perform this operation, you have to set the following environment variables:

- $APPDIR: path of the AppDir you are launching your application from. If this is not present, 
the wrapper will do nothing.

For each environment variable you want restored, where {VAR} is the name of the environment
variable (e.g. "PATH"):
  $APPIMAGE_ORIGINAL_{VAR} -- original value of the environment variable
  $APPIMAGE_STARTUP_{VAR} -- value of the variable when you were starting up
                             your application