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
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "exec_utils.h"
#include "environment.h"
#include "common/path.h"
#include "redirect_path.h"

typedef ssize_t (*execve_func_t)(const char *filename, char *const argv[], char *const envp[]);

static execve_func_t real_execve = NULL;

typedef ssize_t (*execvp_func_t)(const char *filename, char *const argv[]);

static execvp_func_t real_execvp = NULL;

// TODO implement me: execle; but it's annoying work and nothing seems to use them

// typedef int (*execle_func_t)(const char *path, const char *arg, char * const envp[]);
// static execle_func_t old_execle = NULL;

typedef int (*execv_func_t)(const char *path, char *const argv[]);

static execv_func_t real_execv = NULL;

typedef int (*execvpe_func_t)(const char *file, char *const argv[], char *const envp[]);

static execvpe_func_t real_execvpe = NULL;

int execl(const char *filename, const char *args, ...) {
    va_list argv_itr;

    size_t argc = 2; // reserve one for the filename and one for NULL termination

    // count args received
    va_start(argv_itr, args);
    const char *arg_value = NULL;
    while ((arg_value = va_arg(argv_itr, const char *)) != NULL)
        argc++;
    va_end(argv_itr);

    // fill argv
    char *argv[argc];
    va_start(argv_itr, args);

    argv[0] = strdup(args);
    int offset = 1;
    while ((arg_value = va_arg(argv_itr, const char *)) != NULL) {
        argv[offset] = strdup(arg_value);
        offset++;
    }

    argv[offset] = NULL;
    va_end(argv_itr);

    int res = execv(filename, argv);

    // free allocated strings
    for (int i = 0; i < argc; i++)
        free(argv[i]);

    return res;
}

int execlp(const char *filename, const char *args, ...) {
    va_list argv_itr;

    size_t argc = 2; // reserve one for the filename and one for NULL termination

    // count args received
    va_start(argv_itr, args);
    const char *arg_value = NULL;
    while ((arg_value = va_arg(argv_itr, const char *)) != NULL)
        argc++;
    va_end(argv_itr);

    // fill argv
    char *argv[argc];
    va_start(argv_itr, args);

    argv[0] = strdup(args);
    int offset = 1;
    while ((arg_value = va_arg(argv_itr, const char *)) != NULL) {
        argv[offset] = strdup(arg_value);
        offset++;
    }

    argv[offset] = NULL;
    va_end(argv_itr);

    int res = execvp(filename, argv);

    // free allocated strings
    for (int i = 0; i < argc; i++)
        free(argv[i]);

    return res;
}

int execve(const char *filename, char *const argv[], char *const envp[]) {
    char *new_filename = apprun_exec_adjust_path(filename);
    apprun_exec_args_t *new_exec_args = apprun_adjusted_exec_args(new_filename, argv, envp);

    real_execve = dlsym(RTLD_NEXT, "execve");
    int ret = real_execve(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_restore_workdir_if_needed();
    apprun_exec_args_free(new_exec_args);
    free(new_filename);
    return ret;
}


int execv(const char *filename, char *const argv[]) {
    char *new_filename = apprun_exec_adjust_path(filename);
    apprun_exec_args_t *new_exec_args = apprun_adjusted_exec_args(new_filename, argv, environ);

    real_execve = dlsym(RTLD_NEXT, "execve");
    int ret = real_execve(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_restore_workdir_if_needed();
    apprun_exec_args_free(new_exec_args);
    free(new_filename);
    return ret;
}

int execvpe(const char *filename, char *const argv[], char *const envp[]) {
    char *new_filename = apprun_exec_adjust_path(filename);
    apprun_exec_args_t *new_exec_args = apprun_adjusted_exec_args(new_filename, argv, envp);

    real_execvpe = dlsym(RTLD_NEXT, "execvpe");
    int ret = real_execvpe(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_restore_workdir_if_needed();
    apprun_exec_args_free(new_exec_args);
    free(new_filename);
    return ret;
}

int execvp(const char *filename, char *const argv[]) {
    char *new_filename = apprun_exec_adjust_path(filename);
    apprun_exec_args_t *new_exec_args = apprun_adjusted_exec_args(new_filename, argv, environ);

    real_execvpe = dlsym(RTLD_NEXT, "execvpe");
    int ret = real_execvpe(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_restore_workdir_if_needed();
    apprun_exec_args_free(new_exec_args);
    free(new_filename);
    return ret;
}
