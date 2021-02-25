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


#ifndef APPIMAGEEXECWRAPPER_EXECVE_UTILS_H
#define APPIMAGEEXECWRAPPER_EXECVE_UTILS_H

#define APPRUN_USE_BUNDLE_LIBC "APPRUN_USE_BUNDLE_LIBC"

#define APPRUN_ENV_APPDIR "APPDIR"
#define APPRUN_ENV_INTERPRETER "INTERPRETER"

typedef struct {
    char* file;
    char** args;
    char** envp;
} apprun_execve_params_t;

char* apprun_resolve_runtime_interpreter(const char* exec_path);

void apprun_execve_params_free(apprun_execve_params_t* args);

void apprun_print_execve_params(const char* filename, char* const* argv, char* const* envp);

apprun_execve_params_t*
apprun_prepare_execve_params(const char* exec_path, char const* const* user_args, char const* const* user_envp);

#endif //APPIMAGEEXECWRAPPER_EXECVE_UTILS_H
