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
#include <unistd.h>
#include <stdio.h>

#include "path.h"

bool apprun_is_path_child_of(const char *path, const char *base) {
    char *real_base = realpath(base, NULL);
    char *real_path = realpath(path, NULL);

    bool result;

    if (real_base != NULL && real_path != NULL) {
        unsigned int len = strlen(real_base);
        result = strncmp(real_base, real_path, len) == 0;
    } else {
        unsigned int len = strlen(base);
        result = strncmp(base, path, len) == 0;
    }

    if (real_base)
        free(real_base);

    if (real_path)
        free(real_path);
#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: is %s child of %s:  %d\n", path, base, result);
#endif
    return result;
}

void apprun_concat_path(char *dest, const char *path) {
    unsigned long path_len = strlen(path);
    if (path_len == 0)
        return;

    unsigned long dest_len = strlen(dest);
    unsigned long j = dest_len;

    if (j > 0 && dest[j - 1] == '/')
        j--;

    if (dest[j] == '\0')
        dest[j] = '/';

    // copy str avoiding repeated slashes
    for (int i = 0; i <= path_len; i++) {
        if (!(dest[j] == '/' && path[i] == '/'))
            j++;

        dest[j] = path[i];
    }
}
