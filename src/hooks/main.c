#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

/* Trampoline for the real main() */
static int (*main_orig)(int, char **, char **);

void restore_workdir(char **envp) {
    for (char **envp_itr = envp; envp_itr != NULL && *envp_itr != NULL; ++envp_itr) {
        if (strncmp(APPRUN_ENV_ORIG_WORKDIR, *envp_itr, APPRUN_ENV_ORIG_WORKDIR_LEN) == 0) {
            // skip variable name and '='
            char const *orig_workdir = *envp_itr + APPRUN_ENV_ORIG_WORKDIR_LEN + 1;

            chdir(orig_workdir);
#ifdef DEBUG
            fprintf(stderr, "APPRUN_HOOK_DEBUG: restoring workdir %s\n", orig_workdir);
#endif
            break;
        }
    }
}


/* Our fake main() that gets called by __libc_start_main() */
int main_hook(int argc, char **argv, char **envp) {
    restore_workdir(envp);
#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: --- Before main ---\n");
#endif

    int ret = main_orig(argc, argv, envp);
#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: --- After main ----\n");
    fprintf(stderr, "APPRUN_HOOK_DEBUG: main() returned %d\n", ret);
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
    main_orig = main;

    /* Find the real __libc_start_main()... */
    typeof(&__libc_start_main) orig = dlsym(RTLD_NEXT, "__libc_start_main");

    /* ... and call it with our custom main function */
    return orig(main_hook, argc, argv, init, fini, rtld_fini, stack_end);
}