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

#define APPRUN_ENV_APPDIR "APPDIR"
#define APPRUN_ENV_INTERPRETER "INTERPRETER"

typedef struct {
    char* file;
    char** args;
    char** envp;
} apprun_exec_args_t;

void apprun_exec_args_free(apprun_exec_args_t* args);

void apprun_print_exec_args(const char* filename, char* const* argv, char* const* envp);

apprun_exec_args_t* apprun_duplicate_exec_args(const char* filename, char* const* argv);

char **apprun_set_original_workdir_env(char *const *envp);
#endif //APPDIR_RUMTIME_SHARED_H
