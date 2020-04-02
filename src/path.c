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

#include "path.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* appdir_runtime_lookup_next(char* itr, char lookup_char) {
    while (itr != NULL && *itr != '\0' && *itr != lookup_char)
        itr++;

    return itr;
}

char* appdir_runtime_resolve_file_name_from_path(const char* file_name, char* path_env) {
    char* resolved = NULL;
    unsigned file_name_len = strlen(file_name);

    char* itr_begin = path_env;
    while (itr_begin != NULL && *itr_begin != '\0' && resolved == NULL) {
        char* itr_end = itr_begin;
        itr_end = appdir_runtime_lookup_next(itr_end, ':');

        if (itr_end != itr_begin) {
            resolved = calloc(itr_end - itr_begin + file_name_len + 2, sizeof(char));
            strncpy(resolved, itr_begin, itr_end - itr_begin);
            resolved[itr_end - itr_begin] = '/';
            strcat(resolved, file_name);

            if (access(resolved, F_OK) != 0) {
                free(resolved);
                resolved = NULL;
            }

            itr_begin = itr_end;

            // skip separator
            if (itr_begin != '\0')
                itr_begin++;
        }
    }

    return resolved;
}

char* appdir_runtime_resolve_file_name(char const* file_name) {
    char* resolved = NULL;

    resolved = realpath(file_name, NULL);
    if (resolved != NULL)
        return resolved;

    char* path_env = getenv("PATH");
    if (path_env != NULL)
        resolved = appdir_runtime_resolve_file_name_from_path(file_name, path_env);

    return resolved;
}
