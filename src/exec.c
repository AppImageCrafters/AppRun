/**************************************************************************
 *
 * Copyright (C) 2016 Sven Brauch <mail@svenbrauch.de>
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

#define _GNU_SOURCE

#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>

#include "shared.h"

typedef ssize_t (* execve_func_t)(const char* filename, char* const argv[], char* const envp[]);

static execve_func_t real_execve = NULL;

typedef ssize_t (* execvp_func_t)(const char* filename, char* const argv[]);

static execvp_func_t real_execvp = NULL;

// TODO implement me: execl, execlp, execle; but it's annoying work and nothing seems to use them
// typedef int (*execl_func_t)(const char *path, const char *arg);
// static execl_func_t old_execl = NULL;
//
// typedef int (*execlp_func_t)(const char *file, const char *arg);
// static execlp_func_t old_execlp = NULL;
//
// typedef int (*execle_func_t)(const char *path, const char *arg, char * const envp[]);
// static execle_func_t old_execle = NULL;

typedef int (* execv_func_t)(const char* path, char* const argv[]);

static execv_func_t real_execv = NULL;

typedef int (* execvpe_func_t)(const char* file, char* const argv[], char* const envp[]);

static execvpe_func_t real_execvpe = NULL;

int execve(const char* filename, char* const argv[], char* const envp[]) {
#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: %s\n", __PRETTY_FUNCTION__);
#endif

    apprun_exec_args_t* new_exec_args = apprun_adjusted_exec_args(filename, argv, envp);

    real_execve = dlsym(RTLD_NEXT, "execve");
    int ret = real_execve(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_exec_args_free(new_exec_args);
    return ret;
}

int execv(const char* filename, char* const argv[]) {
#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: %s\n", __PRETTY_FUNCTION__);
#endif

    apprun_exec_args_t* new_exec_args = apprun_adjusted_exec_args(filename, argv, environ);

    real_execve = dlsym(RTLD_NEXT, "execve");
    int ret = real_execve(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_exec_args_free(new_exec_args);
    return ret;
}

int execvpe(const char* filename, char* const argv[], char* const envp[]) {
#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: %s\n", __PRETTY_FUNCTION__);
#endif

    apprun_exec_args_t* new_exec_args = apprun_adjusted_exec_args(filename, argv, envp);

    real_execvpe = dlsym(RTLD_NEXT, "execvpe");
    int ret = real_execvpe(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_exec_args_free(new_exec_args);
    return ret;
}

int execvp(const char* filename, char* const argv[]) {
#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: %s\n", __PRETTY_FUNCTION__);
#endif

    apprun_exec_args_t* new_exec_args = apprun_adjusted_exec_args(filename, argv, environ);

    real_execvpe = dlsym(RTLD_NEXT, "execvpe");
    int ret = real_execvpe(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_exec_args_free(new_exec_args);
    return ret;
}
