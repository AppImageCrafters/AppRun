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

/**
This library is intended to be used together with the AppImage distribution mechanism.
Place the library somewhere in your AppImage and point LD_PRELOAD to it
before launching your application.

Whenever your application invokes a child process through execv() or execve(),
this wrapper will intercept the call and see if the child process lies
outside of the bundled appdir. If it does, the wrapper will attempt to undo
any changes done to environment variables before launching the process,
since you probably did not intend to launch it with e.g. the LD_LIBRARY_PATH
you previously set for your application.

To perform this operation, you have to set the following environment variables:
  $APPDIR -- path of the AppDir you are launching your application from. If this
             is not present, the wrapper will do nothing.

For each environment variable you want restored, where {VAR} is the name of the environment
variable (e.g. "PATH"):
  $APPIMAGE_ORIGINAL_{VAR} -- original value of the environment variable
  $APPIMAGE_STARTUP_{VAR} -- value of the variable when you were starting up
                             your application
*/

#define _GNU_SOURCE

#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include "shared.h"
#include "environment.h"

typedef ssize_t (*execve_func_t)(const char* filename, char* const argv[], char* const envp[]);
static execve_func_t old_execve = NULL;

typedef ssize_t (*execvp_func_t)(const char* filename, char* const argv[]);
static execvp_func_t old_execvp = NULL;

// TODO implement me: execl, execlp, execle; but it's annoying work and nothing seems to use them
// typedef int (*execl_func_t)(const char *path, const char *arg);
// static execl_func_t old_execl = NULL;
//
// typedef int (*execlp_func_t)(const char *file, const char *arg);
// static execlp_func_t old_execlp = NULL;
//
// typedef int (*execle_func_t)(const char *path, const char *arg, char * const envp[]);
// static execle_func_t old_execle = NULL;

typedef int (*execv_func_t)(const char *path, char *const argv[]);
static execv_func_t old_execv = NULL;

typedef int (*execvpe_func_t)(const char *file, char *const argv[], char *const envp[]);
static execvpe_func_t old_execvpe = NULL;

int execve(const char* filename, char* const argv[], char* const envp[]) {
    char** new_envp = adjusted_environment(filename, envp);
    old_execve = dlsym(RTLD_NEXT, "execve");
    int ret = old_execve(filename, argv, new_envp);
    appdir_runtime_string_list_free(new_envp);
    return ret;
}

int execv(const char* filename, char* const argv[]) {
    char** new_envp = adjusted_environment(filename, environ);
    old_execve = dlsym(RTLD_NEXT, "execve");
    int ret = old_execve(filename, argv, new_envp);
    appdir_runtime_string_list_free(new_envp);
    return ret;
}

int execvpe(const char* filename, char* const argv[], char* const envp[]) {
    // TODO: might not be full path
    char** new_envp = adjusted_environment(filename, envp);
    old_execvpe = dlsym(RTLD_NEXT, "execvpe");
    int ret = old_execvpe(filename, argv, new_envp);
    appdir_runtime_string_list_free(new_envp);
    return ret;
}

int execvp(const char* filename, char* const argv[]) {
    // TODO: might not be full path
    char** new_envp = adjusted_environment(filename, environ);
    old_execvpe = dlsym(RTLD_NEXT, "execvpe");
    int ret = old_execvpe(filename, argv, new_envp);
    appdir_runtime_string_list_free(new_envp);
    return ret;
}
