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

#define _GNU_SOURCE
#define __USE_GNU

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "redirect_path.h"
#include "exec_args.h"

#define APPRUN_PATH_MAPPINGS "APPRUN_PATH_MAPPINGS"

typedef struct {
    char* path;
    char* mapping;
} apprun_path_mapping;

static apprun_path_mapping** apprun_path_mappings = NULL;
static int apprun_path_mappings_size = 0;

void apprun_load_path_mappings() {
    char* path_mappings_env = getenv(APPRUN_PATH_MAPPINGS);
    if (path_mappings_env == NULL || apprun_path_mappings != NULL) {
        return;
    }

    for (int i = 0; i < strlen(path_mappings_env); i++) {
        if (path_mappings_env[i] == ';')
            apprun_path_mappings_size++;
    }

    apprun_path_mappings = malloc(sizeof(apprun_path_mapping*) * apprun_path_mappings_size);

    char* str_itr = path_mappings_env;
    for (int i = 0; i < apprun_path_mappings_size; i++) {

        char* colon_pos, * semicolon_pos;

        colon_pos = strchr(str_itr, ':');
        semicolon_pos = strchr(str_itr, ';');
        if (colon_pos && semicolon_pos) {
            apprun_path_mapping* mapping;
            mapping = malloc(sizeof(apprun_path_mapping));
            mapping->path = strndup(str_itr, colon_pos - str_itr);
            mapping->mapping = strndup(colon_pos + 1, semicolon_pos - colon_pos - 1);

            apprun_path_mappings[i] = mapping;
        }

        str_itr = semicolon_pos + 1;
    }
}

char* redirect_path_full(const char* pathname, int check_parent, int only_if_absolute) {
    int (* _access)(const char* pathname, int mode);
    char* redirected_pathname;
    int ret;

    if (pathname == NULL) {
        return NULL;
    }

    if (only_if_absolute && pathname[0] != '/') {
        return strdup(pathname);
    }

    char* appdir_env = getenv(APPRUN_ENV_APPDIR);
    if (appdir_env == NULL) {
        return strdup(pathname);
    }

    // ensure that path mappings are loaded
    apprun_load_path_mappings();
    if (apprun_path_mappings_size == 0)
        return strdup(pathname);

    _access = (int (*)(const char* pathname, int mode)) dlsym(RTLD_NEXT, "access");

    redirected_pathname = malloc(PATH_MAX);
    for (int i = 0; i < apprun_path_mappings_size; i++) {
        if (strncmp(pathname, apprun_path_mappings[i]->path, strlen(apprun_path_mappings[i]->path)) == 0) {
            memset(redirected_pathname, 0, PATH_MAX);

            const char* mapping = apprun_path_mappings[i]->mapping;
            const char* path_postfix = pathname + strlen(apprun_path_mappings[i]->path);

            strcat(redirected_pathname, mapping);
            if (mapping[strlen(mapping) - 1] != '/' && path_postfix[0] != '/')
                strcat(redirected_pathname, "/");

            strcat(redirected_pathname, path_postfix);

            ret = _access(redirected_pathname, F_OK);
            if (ret == 0 || errno == ENOTDIR) { // ENOTDIR is OK because it exists at least
                return redirected_pathname;
            }
        }
    }

    free(redirected_pathname);
    return strdup(pathname);
}

char* apprun_redirect_path_if_absolute(const char* pathname) {
    return redirect_path_full(pathname, 0, 1);
}

char* apprun_redirect_path_target(const char* pathname) {
    return redirect_path_full(pathname, 1, 0);
}

char* apprun_redirect_path(const char* pathname) {
    return redirect_path_full(pathname, 0, 0);
}

