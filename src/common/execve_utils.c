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
#include <limits.h>

#include "execve_utils.h"
#include "elf_utils.h"
#include "string_list.h"
#include "environment.h"
#include "shell_utils.h"



char* apprun_resolve_runtime_interpreter(FILE* file, unsigned char* e_ident) {
    char* interpreter_path = apprun_elf_read_pt_interp(file, e_ident);
    bool use_bundle_libc = getenv(APPRUN_USE_BUNDLE_LIBC) != NULL;

    if (use_bundle_libc && interpreter_path) {
        const char* path_parts[] = {getenv("APPDIR"), "opt/libc", interpreter_path, NULL};
        char* bundled_interpreter_path = apprun_string_list_join(path_parts, "/");
        if (access(bundled_interpreter_path, X_OK) == 0) {
            free(interpreter_path);
            return bundled_interpreter_path;
        } else {
            // fallback to the system interpreter if none is bundled
            fprintf(stderr, "APPRUN_ERROR: Bundle GLIBC interpreter required but not found: '%s'",
                    bundled_interpreter_path);
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

void apprun_print_execve_params(const char* filename, const char* const* argv, const char* const* envp) {
    fprintf(stderr, "  filename: \"%s\"\n", filename);
    fprintf(stderr, "  args: [ ");
    if (argv) {
        for (const char* const* itr = argv; *itr != 0; itr++) {
            fprintf(stderr, "\"%s\"", *itr);
            if (*(itr + 1) != NULL)
                fprintf(stderr, ", ");
        }
    }

    fprintf(stderr, "]\n");

    fprintf(stderr, "  envp: [ \n");
    if (envp) {
        for (const char* const* itr = envp; *itr != 0; itr++) {
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
            char* args_string = NULL;
            // +1 to ignore self filename
            if (user_args[0] != NULL)
                args_string= apprun_string_list_join(user_args + 1, " ");

            item->current_value = args_string;
            item->startup_value = args_string;
        }
    }

    return apprun_env_item_list_to_envp(envp_list);
}

char* apprun_read_shebang(FILE* pFile) {
    fseek(pFile, 0, SEEK_SET);
    char buff[1024];
    size_t bytes = fread(buff, sizeof(char), 1024, pFile);
    if (bytes > 2 && buff[0] == '#' && buff[1] == '!') {
        char* line_end = strstr(buff, "\n");
        if (line_end != NULL) {
            char* line_start = buff + 2;
            return strndup(line_start, line_end - line_start);
        }
    }

    return NULL;
}

apprun_execve_params_t*
apprun_execve_params_prepare_bundle(const char* exec_path, const char* const* argv_orig, const char* const* envp_orig) {
    apprun_execve_params_t* params = malloc(sizeof(apprun_execve_params_t));
    FILE* exec_file = fopen(exec_path, "rb");
    unsigned char* elf_e_ident = apprun_elf_require_ident(exec_file);
    if (elf_e_ident != NULL) {
        // elf file
        char* interpreter_path = apprun_resolve_runtime_interpreter(exec_file, elf_e_ident);
        if (interpreter_path != NULL) {
            // this elf file needs the glibc interpreter to run, let's use it as main executable
            params->file = interpreter_path;

            const char* interpreter_prefix[] = {params->file, NULL};
            params->args = apprun_string_list_extend(interpreter_prefix, argv_orig);
        } else {
            // We are in presence of a statically linked or interpreted executable
            params->file = strdup(exec_path);
            params->args = apprun_string_list_dup(argv_orig);
        }
    } else {
        // provably an interpreted executable
        char* shebang = apprun_read_shebang(exec_file);
        if (shebang != NULL) {
            char** shebang_parts = apprun_shell_split_arguments(shebang);
            if (strcmp("/usr/bin/env", shebang_parts[0]) != 0)
                apprun_execve_params_prepare_interpreted_executable_args(params, shebang_parts, argv_orig);

            apprun_string_list_free(shebang_parts);
        }

        free(shebang);
    }

    params->envp = apprun_replace_exec_path_and_args_in_envp(exec_path, argv_orig, envp_orig);
    return params;
}

void
apprun_execve_params_prepare_interpreted_executable_args(apprun_execve_params_t* params, char* const* shebang_parts,
                                                         const char* const* argv_orig) {
    FILE* interpreter_fd = fopen(shebang_parts[0], "rb");
    char* e_ident = apprun_elf_require_ident(interpreter_fd);
    char* interpreter_path = apprun_resolve_runtime_interpreter(interpreter_fd, e_ident);

    params->file = interpreter_path;
    const char* interpreter_prefix[] = {params->file, NULL};

    char** args_prefix = apprun_string_list_extend(interpreter_prefix, shebang_parts);
    params->args = apprun_string_list_extend(args_prefix, argv_orig);

    free(e_ident);
    apprun_string_list_free(args_prefix);
    fclose(interpreter_fd);
}

