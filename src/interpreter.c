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

char const* const APPDIR_RUNTIME_INTERPRETER_ENV = "INTERPRETER";

apprun_exec_args_t* apprun_duplicate_exec_args(const char* filename, char* const* argv) {
    apprun_exec_args_t* result;
    result = calloc(1, sizeof(apprun_exec_args_t));

    // use original filename
    result->file = strdup(filename);

    int array_size = apprun_array_len(argv);
    result->args = calloc(array_size, sizeof(char*));
    char* const* src_itr = argv;
    char** target_itr = result->args;

    // copy arguments
    for (; *src_itr != NULL; src_itr++, target_itr++)
        *target_itr = strdup(*src_itr);

    return result;
}

apprun_exec_args_t*
apprun_prepend_interpreter_to_exec(char const* interpreter, char const* filename, char* const* argv) {
    apprun_exec_args_t* result;
    result = calloc(1, sizeof(apprun_exec_args_t));

    // use original filename
    result->file = strdup(interpreter);


    int array_size = apprun_array_len(argv);

    // alloc array size with an extra space for the interpreter
    result->args = calloc(array_size + 1, sizeof(char*));


    char** target_itr = result->args;

    // place interpreter as first argument, not mandatory but it's a linux convention
    *target_itr = strdup(interpreter);
    target_itr++;

    // place the target filename as second argument
    *target_itr = strdup(filename);
    target_itr++;

    // copy arguments
    for (char* const* src_itr = argv + 1; *src_itr != NULL; src_itr++, target_itr++)
        *target_itr = strdup(*src_itr);

    return result;
}

bool apprun_is_exec_args_change_required(const char* appdir, const char* interpreter, const char* filename) {
    if (interpreter == NULL || appdir == NULL)
        return false;

    return apprun_is_path_child_of(filename, appdir);
}

