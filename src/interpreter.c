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
#include <memory.h>
#include <stdbool.h>

#include "interpreter.h"
#include "shared.h"

appdir_runtime_exec_args_t* appdir_runtime_duplicate_exec_args(const char* filename, char* const* argv) {
    appdir_runtime_exec_args_t* result;
    result = calloc(1, sizeof(appdir_runtime_exec_args_t));

    // use original filename
    result->file = strdup(filename);

    int array_size = appdir_runtime_array_len(argv);
    result->args = calloc(array_size, sizeof(char*));
    char* const* src_itr = argv;
    char** target_itr = result->args;

    // copy arguments
    for (; *src_itr != NULL; src_itr++, target_itr++)
        *target_itr = strdup(*src_itr);

    return result;
}

appdir_runtime_exec_args_t*
appdir_runtime_prepend_interpreter_to_exec(char const* interpreter, char* const* argv) {
    appdir_runtime_exec_args_t* result;
    result = calloc(1, sizeof(appdir_runtime_exec_args_t));

    // use original filename
    result->file = strdup(interpreter);


    int array_size = appdir_runtime_array_len(argv);

    // alloc array size with an extra space for the interpreter
    result->args = calloc(array_size + 1, sizeof(char*));

    char* const* src_itr = argv;
    char** target_itr = result->args;

    // place interpreter as first argument, not mandatory but it's a linux convention
    *target_itr = strdup(interpreter);
    target_itr++;

    // copy arguments
    for (; *src_itr != NULL; src_itr++, target_itr++)
        *target_itr = strdup(*src_itr);

    return result;
}

bool appdir_runtime_is_path_child_of(const char* path, const char* base) {
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

bool appdir_runtime_is_exec_args_change_required(const char* appdir, const char* interpreter, const char* filename) {
    if (interpreter == NULL || appdir == NULL)
        return false;

    return appdir_runtime_is_path_child_of(filename, appdir);
}

appdir_runtime_exec_args_t* appdir_runtime_adjusted_exec_args(const char* filename, char* const* argv) {
    char* appdir = getenv("APPDIR");
    char* interpreter = getenv("INTERPRETER");

    if (appdir_runtime_is_exec_args_change_required(appdir, interpreter, filename))
        return appdir_runtime_prepend_interpreter_to_exec(interpreter, argv);
    else
        return appdir_runtime_duplicate_exec_args(filename, argv);
}

void appdir_runtime_exec_args_free(appdir_runtime_exec_args_t* args) {
    appdir_runtime_string_list_free(args->args);
    free(args->args);
    free(args->file);
    free(args);
}

