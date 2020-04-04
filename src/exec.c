/*
 * Copyright (C) 2016 Sven Brauch <mail@svenbrauch.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

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
