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

#include "shared.h"
#include "interpreter.h"
#include "path.h"
#include "environment.h"

int apprun_array_len(char* const* arr) {
    if (arr)
        return apprun_string_list_len(arr) + 1; // allocate extra space for the 0 termination
    else
        return 0;
}

int apprun_string_list_len(char* const* x) {
    int len = 0;
    while (x[len] != 0)
        len++;

    return len;
}

void apprun_string_list_free(char** string_list) {
    if (string_list != NULL) {
        for (char** itr = string_list; *itr != NULL; itr++)
            free(*itr);

        free(string_list);
    }
}

char** apprun_string_list_alloc(unsigned int size) {
    char** ret = calloc(size, sizeof(char*));
    return ret;
}

bool apprun_is_path_child_of(const char* path, const char* base) {
    char* real_base_path = realpath(base, NULL);
    char* real_path = realpath(path, NULL);

    bool result;

    if (real_base_path != NULL && real_path != NULL) {
        unsigned int len = strlen(real_base_path);
        result = strncmp(real_base_path, real_path, len) == 0;
    } else {
        unsigned int len = strlen(base);
        result = strncmp(base, path, len) == 0;
    }

    if (real_base_path)
        free(real_base_path);

    if (real_path)
        free(real_path);

    return result;
}

void apprun_exec_args_free(apprun_exec_args_t* args) {
    apprun_string_list_free(args->args);
    apprun_string_list_free(args->envp);
    free(args->file);
    free(args);
}


void apprun_print_exec_args(const char* filename, char* const* argv, char* const* envp) {
    fprintf(stderr, "APPRUN_HOOK_DEBUG:\tfilename: \"%s\"\n", filename);
    fprintf(stderr, "APPRUN_HOOK_DEBUG:\targs: [ ");
    if (argv) {
        for (char* const* itr = argv; *itr != 0; itr++) {
            fprintf(stderr, "\"%s\"", *itr);
            if (*(itr + 1) != NULL)
                fprintf(stderr, ", ");
        }
    }

    fprintf(stderr, "]\n");

    fprintf(stderr, "APPRUN_HOOK_DEBUG:\tenvp: [ ");
    if (envp) {
        for (char* const* itr = envp; *itr != 0; itr++) {
            fprintf(stderr, "\"%s\"", *itr);
            if (*(itr + 1) != NULL)
                fprintf(stderr, ", ");
        }
    }


    fprintf(stderr, "]\n");
}

apprun_exec_args_t* apprun_adjusted_exec_args(const char* filename, char* const* argv, char* const* envp) {
    char* resolved_file_name = apprun_resolve_file_name(filename);


    char* appdir = getenv("APPDIR");
    char* interpreter = getenv("INTERPRETER");

#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: APPDIR: %s\n", appdir);
    fprintf(stderr, "APPRUN_HOOK_DEBUG: INTERPRETER: %s\n", interpreter);
    fprintf(stderr, "APPRUN_HOOK_DEBUG: ORIGINAL ARGUMENTS\n");
    apprun_print_exec_args(resolved_file_name, argv, envp);
#endif

    apprun_exec_args_t* res = NULL;
    if (apprun_is_exec_args_change_required(appdir, interpreter, resolved_file_name)) {
#ifdef DEBUG
        fprintf(stderr, "APPRUN_HOOK_DEBUG: PREPENDING APPDIR INTERPRETER\n");
#endif

        res = apprun_prepend_interpreter_to_exec(interpreter, resolved_file_name, argv);
    } else
        res = apprun_duplicate_exec_args(resolved_file_name, argv);


    if (appdir != NULL && apprun_is_path_child_of(resolved_file_name, appdir)) {
#ifdef DEBUG
        fprintf(stderr, "APPRUN_HOOK_DEBUG: REMOVING APPDIR PRIVATE ENVIRONMENT\n");
#endif
        res->envp = apprun_string_list_dup(envp);
    } else
        res->envp = apprun_export_envp(envp);

#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: ADJUSTED ARGUMENTS\n");
    apprun_print_exec_args(res->file, res->args, res->envp);
#endif

    free(resolved_file_name);
    return res;
}

char** apprun_string_list_dup(char* const* envp) {
    if (envp != NULL) {
        unsigned size = apprun_array_len(envp);
        char** copy = apprun_string_list_alloc(size);

        char* const* itr1 = envp;
        char** itr2 = copy;
        for (; *itr1 != NULL; itr1++, itr2++)
            *itr2 = strdup(*itr1);

        return copy;
    } else
        return NULL;
}
