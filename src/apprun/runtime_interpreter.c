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
#include <unistd.h>
#include <memory.h>
#include <limits.h>

#include "common/file_utils.h"
#include "common/string_list.h"
#include "common/shell_utils.h"
#include "hooks/environment.h"

#include "runtime_interpreter.h"
#include "runtime_environment.h"


char *parse_ld_trace_line_path(const char *line) {
    char *path = NULL;
    const char *path_start = strstr(line, "=> ");
    if (path_start != NULL) {
        path_start += 3;
    } else {
        path_start = line;
        while (path_start != NULL && isspace(*path_start))
            path_start++;
    }

    char *path_end = strstr(path_start, " (");

    if (path_end != NULL)
        path = strndup(path_start, path_end - path_start);
    else
        path = strdup(path_start);


    return path;
}

bool is_linker_version_string_valid(char *buff) {
    unsigned buff_len = strlen(buff);
    return (isdigit(buff[0]) && isdigit(buff[buff_len - 1]));
}


char *try_read_ld_version_string(FILE *fp) {
    char glibc_version_prefix[] = "ld-";
    int c = 0;
    for (int i = 1; i < strlen(glibc_version_prefix); i++) {
        c = fgetc(fp);
        if (c != glibc_version_prefix[i])
            return 0;
    }

    char buff[256] = {0x0};
    for (int i = 0; i < 256; i++) {
        c = fgetc(fp);
        if (c == '\0' || c == -1)
            break;

        buff[i] = c;
    }

    char *suffix_idx = strstr(buff, ".so");
    if (suffix_idx)
        *suffix_idx = 0;

    if (is_linker_version_string_valid(buff))
        return strdup(buff);

    return NULL;
}

char *read_ld_version(char *path) {
    char version[254] = {0x0};
    FILE *fp = fopen(path, "r");
    if (fp) {
        int itr;
        while ((itr = fgetc(fp)) != EOF) {
            if (itr == 'l') {
                char *new_version = try_read_ld_version_string(fp);
                if (new_version != NULL) {
                    if (compare_version_strings(new_version, version) > 0)
                        strcpy(version, new_version);

                    free(new_version);
                }
            }
        }

        fclose(fp);
    }

    return strdup(version);
}

long compare_version_strings(char *a, char *b) {
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
#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: using appdir libc\n");
#endif
    char *ld_library_path = apprun_shell_expand_variables("$APPDIR_LIBRARY_PATH:$LIBC_LIBRARY_PATH:"
                                                          "$"APPRUN_ENV_ORIG_PREFIX"LD_LIBRARY_PATH", NULL);

    apprun_env_set("LD_LIBRARY_PATH", ld_library_path, "", ld_library_path);
    free(ld_library_path);
}

void configure_system_libc() {
#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: using system libc\n");
#endif
    char *ld_library_path = apprun_shell_expand_variables("$APPDIR_LIBRARY_PATH:"
                                                          "$"APPRUN_ENV_ORIG_PREFIX"LD_LIBRARY_PATH", NULL);

    apprun_env_set("LD_LIBRARY_PATH", ld_library_path, "", ld_library_path);
    free(ld_library_path);
}

char *require_environment(char *name) {
    char *value = getenv(name);
    if (value == NULL) {
        fprintf(stderr, "APPRUN ERROR: Missing %s environment", name);
        exit(1);
    }

    return value;
}

void setup_runtime() {
    char *linkers = strdup(getenv(LD_PATHS_ENV));
    char *ld_relpath = strtok(linkers, LD_PATHS_ENV_SEPARATOR);
    if (ld_relpath != NULL) {
        char *compat_ld_path = resolve_linker_path(ld_relpath, COMPAT_RUNTIME_PREFIX);
        char *default_ld_path = resolve_linker_path(ld_relpath, DEFAULT_RUNTIME_PREFIX);

        char *system_ld_version = read_ld_version(default_ld_path);
        char *appdir_ld_version = read_ld_version(compat_ld_path);

#ifdef DEBUG
        fprintf(stderr, "APPRUN_DEBUG: interpreter \"%s\" \n", strrchr(ld_relpath, '/') + 1);
        fprintf(stderr, "APPRUN_DEBUG: system ld(%s), appdir ld(%s) \n", system_ld_version, appdir_ld_version);
#endif
        char *runtime_path = NULL;
        if (compare_version_strings(system_ld_version, appdir_ld_version) > 0) {
            runtime_path = resolve_runtime_path(DEFAULT_RUNTIME_PREFIX);
            configure_system_libc();
        } else {
            runtime_path = resolve_runtime_path(COMPAT_RUNTIME_PREFIX);
            configure_embed_libc();
        }


        apprun_env_set(APPRUN_ENV_RUNTIME, runtime_path, "", runtime_path);

        char cwd[PATH_MAX];
        getcwd(cwd, PATH_MAX);
        setenv(APPRUN_ENV_ORIGINAL_WORKDIR, cwd, 1);
        chdir(runtime_path);
    }
}

char *resolve_runtime_path(const char *prefix) {
    char *appdir = require_environment("APPDIR");
    int appdir_len = strlen(appdir);
    int runtime_prefix_len = strlen(prefix);

    int path_len = appdir_len + runtime_prefix_len + 1;
    char *path = calloc(path_len, sizeof(char));
    memset(path, 0, path_len);
    strcat(path, appdir);
    strcat(path, prefix);

    return path;
}

char *resolve_linker_path(char *relpath, char const *prefix) {
    char *appdir = require_environment("APPDIR");
    int appdir_len = strlen(appdir);
    int runtime_prefix_len = strlen(prefix);
    int relpath_len = strlen(relpath);

    int path_len = appdir_len + runtime_prefix_len + relpath_len + 1;
    char *path = calloc(path_len, sizeof(char));
    memset(path, 0, path_len);
    strcat(path, appdir);
    strcat(path, prefix);
    strcat(path, relpath);

    return path;
}

