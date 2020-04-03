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

#include <stdlib.h>
#include <string.h>

#include "shared.h"

int appdir_runtime_array_len(char* const* x) {
    // allocate extra space for the 0 termination
    return appdir_runtime_string_list_len(x) + 1;
}

int appdir_runtime_string_list_len(char* const* x) {
    int len = 0;
    while (x[len] != 0)
        len++;

    return len;
}

void appdir_runtime_string_list_free(char** string_list) {
    if (string_list != NULL) {
        for (char** itr = string_list; *itr != NULL; itr++)
            free(*itr);

        free(string_list);
    }
}

char** appdir_runtime_string_list_alloc(int size) {
    char** ret = calloc(size, sizeof(char*));
    return ret;
}

bool appdir_runtime_is_path_child_of(const char* path, const char* base) {
    char* real_base_path = realpath(base, NULL);
    char* real_path = realpath(path, NULL);

    bool result;

    if (real_base_path != NULL && real_path != NULL) {
        unsigned int len = strlen(real_base_path);
        result = strncmp(real_base_path, real_path, len) == 0;
    } else {
        unsigned int len = strlen(base);
        result = strncmp(base, path, len) == 0;
    }

    if (real_base_path)
        free(real_base_path);

    if (real_path)
        free(real_path);

    return result;
}