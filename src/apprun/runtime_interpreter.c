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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "common/file_utils.h"
#include "common/string_list.h"
#include "common/shell_utils.h"
#include "hooks/environment.h"

#include "runtime_interpreter.h"


char* parse_ld_trace_line_path(const char* line) {
    char* path = NULL;
    const char* path_start = strstr(line, "=> ");
    if (path_start != NULL) {
        path_start += 3;
    } else {
        path_start = line;
        while (path_start != NULL && isspace(*path_start))
            path_start++;
    }

    char* path_end = strstr(path_start, " (");

    if (path_end != NULL)
        path = strndup(path_start, path_end - path_start);
    else
        path = strdup(path_start);


    return path;
}

bool is_glibc_version_string_valid(char* buff) {
    unsigned buff_len = strlen(buff);
    return (isdigit(buff[0]) && isdigit(buff[buff_len - 1]));
}


char* try_read_glibc_version_string(FILE* fp) {
    char glibc_version_prefix[] = "GLIBC_";
    for (int i = 1; i < strlen(glibc_version_prefix); i++) {
        if (fgetc(fp) != glibc_version_prefix[i])
            return NULL;
    }

    char buff[256] = {0x0};
    for (int i = 0; i < 256; i++) {
        int c = fgetc(fp);
        if (c == '\0' || c == -1)
            break;

        buff[i] = c;
    }

    if (is_glibc_version_string_valid(buff))
        return strdup(buff);

    return NULL;
}

char* read_libc_version(char* path) {
    char version[254] = {0x0};
    FILE* fp = fopen(path, "r");
    if (fp) {
        int itr;
        while ((itr = fgetc(fp)) != EOF) {
            if (itr == 'G') {
                char* new_version = try_read_glibc_version_string(fp);
                if (new_version != NULL) {
                    if (compare_glib_version_strings(new_version, version) > 0)
                        strcpy(version, new_version);

                    free(new_version);
                }
            }
        }

        fclose(fp);
    }

    return strdup(version);
}

long compare_glib_version_strings(char* a, char* b) {
    if (a == NULL || b == NULL)
        return a - b;

    long a_vals[3] = {0x0};
    long b_vals[3] = {0x0};

    sscanf(a, "%ld.%ld.%ld", a_vals, a_vals + 1, a_vals + 2);
    sscanf(b, "%ld.%ld.%ld", b_vals, b_vals + 1, b_vals + 2);

    for (int i = 0; i < 3; i++) {
        long diff = a_vals[i] - b_vals[i];
        if (diff != 0)
            return diff;
    }

    return 0;
}

void configure_embed_libc() {
    char* ld_library_path = apprun_shell_expand_variables("$APPDIR_LIBRARY_PATH:$LIBC_LIBRARY_PATH:"
                                                          "$"APPRUN_ENV_ORIG_PREFIX"LD_LIBRARY_PATH", NULL);
    setenv("LD_LIBRARY_PATH", ld_library_path, 1);
    setenv(APPRUN_ENV_STARTUP_PREFIX"LD_LIBRARY_PATH", ld_library_path, 1);
    free(ld_library_path);
}

void configure_system_libc() {
    char* ld_library_path = apprun_shell_expand_variables("$APPDIR_LIBRARY_PATH:"
                                                          "$"APPRUN_ENV_ORIG_PREFIX"LD_LIBRARY_PATH", NULL);
    setenv("LD_LIBRARY_PATH", ld_library_path, 1);
    setenv(APPRUN_ENV_STARTUP_PREFIX"LD_LIBRARY_PATH", ld_library_path, 1);
    free(ld_library_path);
}

char* require_environment(char* name) {
    char* value = getenv(name);
    if (value == NULL) {
        fprintf(stderr, "APPRUN ERROR: Missing %s environment", name);
        exit(1);
    }

    return value;
}

char* resolve_libc_from_interpreter_path(char* path) {
#define LIBC_SO_NAME "libc.so.6"

    char* r_path = realpath(path, NULL);
    char* split = strrchr(r_path, '/');
    split++; // include the '/'

    char* libc_path = calloc(strlen(LIBC_SO_NAME) + split - r_path + 1, sizeof(char));
    strncat(libc_path, r_path, split - r_path);
    strcat(libc_path, LIBC_SO_NAME);

    free(r_path);
    return libc_path;
}

void setup_interpreter() {
    char* system_interpreter_path = require_environment("SYSTEM_INTERP");
    char* system_libc_path = resolve_libc_from_interpreter_path(system_interpreter_path);


    char* system_libc_version = read_libc_version(system_libc_path);
    char* appdir_libc_version = require_environment("APPDIR_LIBC_VERSION");

#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: system glibc(%s), appdir glibc(%s) \n", system_libc_version, appdir_libc_version);
#endif
    if (compare_glib_version_strings(system_libc_version, appdir_libc_version) > 0) {
        configure_system_libc();
        deploy_interpreter(system_interpreter_path);
    } else {
        configure_embed_libc();
        char* appdir_interpreter_path = require_environment("APPDIR_INTERP");
        deploy_interpreter(appdir_interpreter_path);
    }

    free(system_libc_path);
}

void deploy_interpreter(char* path) {
    char* target_path = require_environment("RUNTIME_INTERP");
    apprun_file_copy(path, target_path);

    chmod(target_path, S_IRWXU | S_IRWXG);
}

