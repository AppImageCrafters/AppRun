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

#ifndef APPDIR_RUMTIME_SHARED_H
#define APPDIR_RUMTIME_SHARED_H

#include <stdbool.h>

typedef struct {
    char* file;
    char** args;
    char** envp;
} apprun_exec_args_t;

apprun_exec_args_t* apprun_adjusted_exec_args(const char* filename, char* const* argv, char* const* envp);

void apprun_exec_args_free(apprun_exec_args_t* args);

char** apprun_string_list_alloc(unsigned int size);

char** apprun_string_list_dup(char* const* envp);
void apprun_string_list_free(char** string_list);

int apprun_string_list_len(char* const* x);

int apprun_array_len(char* const* arr);

bool apprun_is_path_child_of(const char* path, const char* base);

#endif //APPDIR_RUMTIME_SHARED_H
