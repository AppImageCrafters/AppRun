/**************************************************************************
 *
 * Copyright (c) 2020 Alexis Lopez Zubieta <contact@azubieta.net>
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <linux/limits.h>
#include <unistd.h>

#include "common/string_list.h"
#include "common/path.h"
#include "exec_utils.h"
#include "environment.h"


void apprun_exec_args_free(apprun_exec_args_t *args) {
    apprun_string_list_free(args->args);
    apprun_string_list_free(args->envp);
    free(args->file);
    free(args);
}


void apprun_print_exec_args(const char *filename, char *const *argv, char *const *envp) {
    fprintf(stderr, "  filename: \"%s\"\n", filename);
    fprintf(stderr, "  args: [ ");
    if (argv) {
        for (char *const *itr = argv; *itr != 0; itr++) {
            fprintf(stderr, "\"%s\"", *itr);
            if (*(itr + 1) != NULL)
                fprintf(stderr, ", ");
        }
    }

    fprintf(stderr, "]\n");

    fprintf(stderr, "  envp: [ \n");
    if (envp) {
        for (char *const *itr = envp; *itr != 0; itr++) {
            fprintf(stderr, "    \"%s\"", *itr);
            if (*(itr + 1) != NULL)
                fprintf(stderr, ", \n");
        }
    }


    fprintf(stderr, "]\n");
}


apprun_exec_args_t *apprun_duplicate_exec_args(const char *filename, char *const *argv) {
    apprun_exec_args_t *result;
    result = calloc(1, sizeof(apprun_exec_args_t));

    // use original filename
    result->file = strdup(filename);

    int array_size = apprun_array_len(argv);
    result->args = calloc(array_size, sizeof(char *));
    char *const *src_itr = argv;
    char **target_itr = result->args;

    // copy arguments
    for (; *src_itr != NULL; src_itr++, target_itr++)
        *target_itr = strdup(*src_itr);

    return result;
}

char **apprun_set_original_workdir_env(char *const *envp) {
    char cwd_path[PATH_MAX] = {0x0};
    getcwd(cwd_path, PATH_MAX);


    char **new_envp = apprun_envp_set(APPRUN_ENV_ORIGINAL_WORKDIR, cwd_path, envp);
#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: storing original workdir %s\n", cwd_path);
#endif

    return new_envp;
}

void chdir_to_runtime() {
    char const *runtime_path = getenv(APPRUN_ENV_RUNTIME);
    if (runtime_path != NULL) {
        chdir(runtime_path);
    } else {
        fprintf(stderr,
                "APPRUN_HOOK_WARNING: %s environment variable not set, execution of bundled binaries may fail!\n",
                APPRUN_ENV_RUNTIME);
    }
}

apprun_exec_args_t *apprun_adjusted_exec_args(const char *filename, char *const *argv, char *const *envp) {
    char *appdir = getenv("APPDIR");

#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: APPDIR: %s\n", appdir);
    fprintf(stderr, "APPRUN_HOOK_DEBUG: ORIGINAL EXEC_ARGS\n");
    apprun_print_exec_args(filename, argv, envp);
#endif

    apprun_exec_args_t *res = NULL;
    res = apprun_duplicate_exec_args(filename, argv);

    int is_nested_path = apprun_is_path_child_of(filename, appdir);
    if (appdir != NULL && is_nested_path) {
#ifdef DEBUG
        fprintf(stderr, "APPRUN_HOOK_DEBUG: USING BUNDLE RUNTIME\n");
#endif
        res->envp = apprun_set_original_workdir_env(envp);
        chdir_to_runtime();
    } else {
#ifdef DEBUG
        fprintf(stderr, "APPRUN_HOOK_DEBUG: USING SYSTEM RUNTIME\n");
#endif
        res->envp = apprun_export_envp(envp);
    }

#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: APPDIR: %s\n", appdir);
    fprintf(stderr, "APPRUN_HOOK_DEBUG: REAL EXEC_ARGS\n");
    apprun_print_exec_args(res->file, res->args, res->envp);
#endif

    return res;
}
