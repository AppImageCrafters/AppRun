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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "runtime_environment.h"

#include "common/shell_utils.h"
#include "common/string_list.h"
#include "hooks/environment.h"


void apprun_update_env(const char *name, const char *value) {
    if (value)
        setenv(name, value, 1);
    else
        unsetenv(name);
}

void apprun_env_set(const char *name, const char *value, const char *orig_value, const char *start_value) {
    apprun_update_env(name, value);

    char *orig_name = apprun_prefix_str(APPRUN_ENV_ORIG_PREFIX, name);
    apprun_update_env(orig_name, orig_value);


    char *startup_name = apprun_prefix_str(APPRUN_ENV_STARTUP_PREFIX, name);
    apprun_update_env(startup_name, start_value);

#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: set env %s=%s\n", name, value);
#endif

    free(orig_name);
    free(startup_name);
}

void apprun_load_env_file(const char *path, char **argv) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "APPRUN ERROR: Unable to open file: %s", path);
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        if (len > 0 && line[0] != '#') {
            // Remove trailing new line chars
            while (read > 0 && line[read - 1] == '\n') {
                line[read - 1] = '\0';
                read--;
            }

            char *name = apprun_env_str_entry_extract_name(line);
            char *value = apprun_env_str_entry_extract_value(line);
            char *expanded_value = apprun_shell_expand_variables(value, argv);

            apprun_env_set(name, expanded_value, getenv(name), expanded_value);

            free(expanded_value);
            free(value);
            free(name);
        }
    }

    free(line);
}

