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
