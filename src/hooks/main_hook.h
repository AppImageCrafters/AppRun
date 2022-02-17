#ifndef APPIMAGEEXECWRAPPER_MAIN_HOOK_H
#define APPIMAGEEXECWRAPPER_MAIN_HOOK_H

/**
 * @brief handle dynamic linker setup at runtime
 *
 * Shared linked binaries may require a linker, a copy libc, and other related libs to properly run. To allow running a
 * binary linked to a newer libc version (i.e. glibc 2.31) in a system with an older libc (i.e.: glibc 2.27) we need to
 * use the newest libc. Therefore an AppDir may include a copy of the libc it requires.
 *
 * Two environments are be defined at build time and the required one is selected at runtime:
 *  - default ($APPDIR/runtime/default): uses the system dynamic linker, libc, related libraries
 *  - compat ($APPDIR/runtime/compat): uses the bundle dynamic linker, libc, related libraries
 *
 * Binaries PT_INTERP and R_PATH sections must be patched at build time to use relative paths. At runtime the AppRun
 * or the exec hooks will change the current dir to runtime dir (pointed by the APPRUN_RUNTIME environment variable)
 * so the relative paths become valid. The real work dir will be restored from the main function hook using the value
 * stored at APPRUN_OWD;
 * */

#define APPRUN_ENV_RUNTIME "APPRUN_RUNTIME"
#define APPRUN_ENV_ORIG_WORKDIR "APPRUN_OWD"
#define APPRUN_ENV_ORIG_WORKDIR_LEN 10

#endif //APPIMAGEEXECWRAPPER_MAIN_HOOK_H
