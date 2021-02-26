/**************************************************************************
 *
 * Copyright (c) 2021 Alexis Lopez Zubieta <contact@azubieta.net>
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



#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "execve_utils.h"
#include "elf_utils.h"
#include "string_list.h"
#include "environment.h"

char* apprun_resolve_runtime_interpreter(const char* exec_path) {
    char* interpreter_path = apprun_elf_read_pt_interp(exec_path);
    bool use_bundle_libc = getenv(APPRUN_USE_BUNDLE_LIBC) != NULL;

    if (use_bundle_libc) {
        const char* path_parts[] = {getenv("APPDIR"), "opt/libc", interpreter_path, NULL};
        char* bundled_interpreter_path = apprun_string_list_join(path_parts, "/");
        if (access(bundled_interpreter_path, X_OK) == 0) {
            free(interpreter_path);
            return bundled_interpreter_path;
        } else {
            // fallback to the system interpreter if none is bundled
            fprintf(stderr, "APPRUN_ERROR: Unable to find bundled interpreter at %s", bundled_interpreter_path);
            free(bundled_interpreter_path);
        }
    }

    return interpreter_path;
}

void apprun_execve_params_free(apprun_execve_params_t* args) {
    apprun_string_list_free(args->args);
    apprun_string_list_free(args->envp);
    free(args->file);
    free(args);
}

void apprun_print_execve_params(const char* filename, char* const* argv, char* const* envp) {
    fprintf(stderr, "  filename: \"%s\"\n", filename);
    fprintf(stderr, "  args: [ ");
    if (argv) {
        for (char* const* itr = argv; *itr != 0; itr++) {
            fprintf(stderr, "\"%s\"", *itr);
            if (*(itr + 1) != NULL)
                fprintf(stderr, ", ");
        }
    }

    fprintf(stderr, "]\n");

    fprintf(stderr, "  envp: [ \n");
    if (envp) {
        for (char* const* itr = envp; *itr != 0; itr++) {
            fprintf(stderr, "    \"%s\"", *itr);
            if (*(itr + 1) != NULL)
                fprintf(stderr, ", \n");
        }
    }


    fprintf(stderr, "]\n");
}

char** apprun_replace_exec_path_and_args_in_envp(const char* exec_path, const char* const* user_args,
                                                 const char* const* user_envp) {
    apprun_env_item_list_t* envp_list = apprun_env_item_list_from_envp(user_envp);
    for (apprun_env_item_list_t* itr = envp_list; itr != NULL && *itr != NULL; itr++) {
        apprun_env_item_t* item = *itr;
        if (strncmp("EXEC_PATH", item->name, 9) == 0) {
            item->current_value = strdup(exec_path);
            item->startup_value = strdup(exec_path);
        }
        if (strncmp("EXEC_ARGS", item->name, 9) == 0) {
            // +1 to ignore self filename
            char* args_string = apprun_string_list_join(user_args + 1, " ");

            item->current_value = args_string;
            item->startup_value = args_string;
        }
    }

    return apprun_env_item_list_to_envp(envp_list);
}

apprun_execve_params_t*
apprun_execve_params_prepare_bundle(const char* exec_path, const char* const* argv_orig, const char* const* envp_orig) {
    apprun_execve_params_t* params = malloc(sizeof(apprun_execve_params_t));
    params->file = apprun_resolve_runtime_interpreter(exec_path);

    // we are in presence of an dynamically linked binary so we prefix the interpreter to our execve call
    const char* interpreter_prefix[] = {params->file, NULL};
    params->args = apprun_string_list_extend(interpreter_prefix, argv_orig);
    params->envp = apprun_replace_exec_path_and_args_in_envp(exec_path, argv_orig, envp_orig);

    return params;
}
