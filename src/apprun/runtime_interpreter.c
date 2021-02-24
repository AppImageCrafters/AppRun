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

#include "common/string_list.h"

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


char* resolve_libc_from_interpreter_path(char* path) {
#define LIBC_SO_NAME "libc.so.6"

    char* r_path = realpath(path, NULL);
    if (r_path != NULL) {
        char* split = strrchr(r_path, '/');
        split++; // include the '/'

        char* libc_path = calloc(strlen(LIBC_SO_NAME) + split - r_path + 1, sizeof(char));
        strncat(libc_path, r_path, split - r_path);
        strcat(libc_path, LIBC_SO_NAME);

        free(r_path);
        return libc_path;
    } else {
        fprintf(stderr, "APPRUN_WARNING: Unable to find interpreter: %s\n", path);
        return NULL;
    }
}

