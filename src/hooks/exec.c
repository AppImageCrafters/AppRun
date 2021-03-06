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

#include "exec_args.h"
#include "interpreter.h"
#include "environment.h"
#include "common/path.h"
#include "common/string_list.h"
#include "redirect_path.h"

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

int apprun_is_exported_binary(const char* new_filename);


apprun_exec_args_t* apprun_adjusted_exec_args(const char* filename, char* const* argv, char* const* envp) {
    char* resolved_filename = apprun_resolve_bin_path(filename);
    char* new_filename = apprun_redirect_path(resolved_filename);
    free(resolved_filename);


    char* appdir = getenv("APPDIR");

#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: APPDIR: %s\n", appdir);
    fprintf(stderr, "APPRUN_HOOK_DEBUG: ORIGINAL EXEC_ARGS\n");
    apprun_print_exec_args(filename, argv, envp);
#endif

    apprun_exec_args_t* res = NULL;
    res = apprun_duplicate_exec_args(new_filename, argv);

    int is_exported_binary = apprun_is_exported_binary(new_filename);
    int is_nested_path = apprun_is_path_child_of(new_filename, appdir);
    if (appdir != NULL && (is_exported_binary || is_nested_path)) {
#ifdef DEBUG
        fprintf(stderr, "APPRUN_HOOK_DEBUG: USING BUNDLE RUNTIME\n");
#endif
        res->envp = apprun_string_list_dup(envp);
    } else {
#ifdef DEBUG
        fprintf(stderr, "APPRUN_HOOK_DEBUG: USING SYSTEM RUNTIME\n");
#endif
        res->envp = apprun_export_envp(envp);
    }
    fflush(stderr);

#ifdef DEBUG
    fprintf(stderr, "APPRUN_HOOK_DEBUG: APPDIR: %s\n", appdir);
    fprintf(stderr, "APPRUN_HOOK_DEBUG: REAL EXEC_ARGS\n");
    apprun_print_exec_args(res->file, res->args, res->envp);
#endif

    free(new_filename);
    return res;
}

int apprun_is_exported_binary(const char* new_filename) {
    char exported_file_prefix[100] = {0x0};
    strcat(exported_file_prefix, "/tmp/appimage-");
    char* uuid = getenv("APPIMAGE_UUID");
    if (uuid != NULL) {
        unsigned uuid_len = strlen(uuid);
        unsigned len = 36 < uuid_len ? 36 : uuid_len;
        strncat(exported_file_prefix, uuid, len);
    }

    int is_exported_binary = (strncmp(exported_file_prefix, new_filename, strlen(exported_file_prefix)) == 0);
#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: is_exported_binary %d\n", is_exported_binary);
#endif
    return is_exported_binary;
}

int execve(const char* filename, char* const argv[], char* const envp[]) {
    apprun_exec_args_t* new_exec_args = apprun_adjusted_exec_args(filename, argv, envp);

    real_execve = dlsym(RTLD_NEXT, "execve");
    int ret = real_execve(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_exec_args_free(new_exec_args);
    return ret;
}

int execv(const char* filename, char* const argv[]) {
    apprun_exec_args_t* new_exec_args = apprun_adjusted_exec_args(filename, argv, environ);

    real_execve = dlsym(RTLD_NEXT, "execve");
    int ret = real_execve(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_exec_args_free(new_exec_args);
    return ret;
}

int execvpe(const char* filename, char* const argv[], char* const envp[]) {
    apprun_exec_args_t* new_exec_args = apprun_adjusted_exec_args(filename, argv, envp);

    real_execvpe = dlsym(RTLD_NEXT, "execvpe");
    int ret = real_execvpe(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_exec_args_free(new_exec_args);
    return ret;
}

int execvp(const char* filename, char* const argv[]) {
    apprun_exec_args_t* new_exec_args = apprun_adjusted_exec_args(filename, argv, environ);

    real_execvpe = dlsym(RTLD_NEXT, "execvpe");
    int ret = real_execvpe(new_exec_args->file, new_exec_args->args, new_exec_args->envp);

    apprun_exec_args_free(new_exec_args);
    return ret;
}
