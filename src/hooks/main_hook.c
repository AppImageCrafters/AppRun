#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/limits.h>

#include "environment.h"

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


/* Trampoline for the real main() */
static int (*apprun_main_orig)(int, char **, char **);

void apprun_restore_workdir() {
    char const *original_pwd = getenv(APPRUN_ENV_ORIGINAL_WORKDIR);
    if (original_pwd != NULL) {
#ifdef DEBUG
        fprintf(stderr, "APPRUN_HOOK_DEBUG: restoring original workdir %s\n", original_pwd);
        fflush(stderr);
#endif

        chdir(original_pwd);
    }
}


/* Wrapper for main() that gets called by __libc_start_main() */
int apprun_main_hook(int argc, char **argv, char **envp) {
    apprun_restore_workdir();
#ifdef DEBUG
    char exec_path[PATH_MAX] = {0x0};
    realpath("/proc/self/exe", exec_path);

    fprintf(stderr, "APPRUN_HOOK_DEBUG: --- Before main %s ---\n", exec_path);
#endif

    int ret = apprun_main_orig(argc, argv, envp);

#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: --- After main ----\n");
    fprintf(stderr, "APPRUN_HOOK_DEBUG: main() returned %d\n", ret);
    fflush(stderr);
#endif

    return ret;
}


/*
 * Wrapper for __libc_start_main() that replaces the real main
 * function with our hooked version.
 */
int __libc_start_main(
        int (*main)(int, char **, char **),
        int argc,
        char **argv,
        int (*init)(int, char **, char **),
        void (*fini)(void),
        void (*rtld_fini)(void),
        void *stack_end) {
    /* Save the real main function address */
    apprun_main_orig = main;

    /* Find the real __libc_start_main()... */
    typeof(&__libc_start_main) orig = dlsym(RTLD_NEXT, "__libc_start_main");

    /* ... and call it with our custom main function */
    return orig(apprun_main_hook, argc, argv, init, fini, rtld_fini, stack_end);
}