/**************************************************************************
 *
 * Copyright (c) 2022 Alexis Lopez Zubieta <contact@azubieta.net>
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

#include "libc_utils.h"


#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <linux/limits.h>
#include <regex.h>

char* apprun_read_glibc_path_from_so_cache() {
    FILE* cache_fd = fopen("/etc/ld.so.cache", "r");
    char libc_path[PATH_MAX] = {0x0};
    bool libc_path_found = false;

    if (cache_fd) {
        char* libc_soname = "libc.so.6";
        size_t libc_soname_len = strlen(libc_soname);

        size_t read_idx = 0;

        int c = fgetc(cache_fd);
        while (c != EOF && !libc_path_found) {
            libc_path[read_idx++] = (char) c;
            if (c == '\0') {
                if ((read_idx >= (libc_soname_len + 1))
                    && (libc_path[0] == '/')
                    && (strncmp(libc_path + (read_idx - libc_soname_len) - 1, libc_soname, libc_soname_len) == 0)
                        ) {
                    libc_path_found = true;
                }
            }

            // reset buffer if char is not printable or the buffer is about to overflow
            if (!isprint(c) || read_idx == PATH_MAX)
                read_idx = 0;

            c = fgetc(cache_fd);
        }

        fclose(cache_fd);
    }


    if (libc_path_found)
        return strdup(libc_path);
    else
        return NULL;
}

char* apprun_read_glibc_version_from_lib(const char* libc_path) {

    char libc_version[PATH_MAX] = {0x0};
    bool libc_version_found = false;

    regex_t regex;
    int regex_comp_res = regcomp(&regex, GLIBC_VERSION_STRING_REGEX, REG_EXTENDED);
    if (regex_comp_res != 0) {
        fprintf(stderr, "APPRUN_ERROR: regcomp failed with code: %d\n", regex_comp_res);
        return NULL;
    }

    FILE* cache_fd = fopen(libc_path, "r");
    if (cache_fd) {
        size_t libc_version_min_size = 6;
        size_t read_idx = 0;

        int c = fgetc(cache_fd);
        while (c != EOF && !libc_version_found) {
            libc_version[read_idx++] = (char) c;
            if (c == '\0') {
                if (read_idx >= libc_version_min_size && regexec(&regex, libc_version, 0, NULL, 0) == 0) {
                    libc_version_found = true;
                }
            }

            // reset buffer if char is not printable or the buffer is about to overflow
            if (!isprint(c) || read_idx == PATH_MAX)
                read_idx = 0;

            c = fgetc(cache_fd);
        }

        fclose(cache_fd);
    }


    if (libc_version_found) {
        size_t glibc_prefix_len = 6;
        return strdup(libc_version + glibc_prefix_len);
    } else
        return NULL;
}
