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

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shell_utils.h"
#include "string_utils.h"

bool apprun_shell_is_var_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

const char* apprun_shell_find_var_start(const char* itr) {
    while (*itr != '\0' && *itr != '$')
        itr++;

    return itr;
}

const char* apprun_shell_find_var_end(const char* itr) {
    if (*itr == '$')
        itr++;

    bool has_brakets = *itr == '{';
    if (has_brakets)
        itr++;

    while (apprun_shell_is_var_char(*itr))
        itr++;

    if (has_brakets)
        itr++;

    return itr;
}

char* apprun_shell_extract_var_name(const char* itr) {
    if (*itr == '$')
        itr++;

    bool has_brakets = *itr == '{';
    if (has_brakets)
        itr++;

    char const* being = itr;

    while (apprun_shell_is_var_char(*itr))
        itr++;


    return strndup(being, itr - being);
}

char* apprun_shell_expand_variables(char const* str) {
    if (str == NULL)
        return NULL;

    unsigned buffer_capacity = 1;
    unsigned buffer_len = 0;
    char* buffer = calloc(buffer_capacity, sizeof(char*));

    char const* itr = str;

    while (*itr != '\0') {
        char const* var_start = apprun_shell_find_var_start(itr);
        char const* var_end = apprun_shell_find_var_end(var_start);


        if (var_start > itr) {
            unsigned section_len = var_start - itr;
            if (buffer_len + section_len > buffer_capacity) {
                buffer_capacity = buffer_capacity + section_len;
                buffer = apprun_string_extend(buffer, buffer_capacity);
            }

            strncat(buffer, itr, section_len);
            buffer_len += section_len;
        }


        if (var_start != var_end) {
            char* var_name = apprun_shell_extract_var_name(var_start);
            char* var_value = getenv(var_name);
            free(var_name);

            unsigned section_len = strlen(var_value);
            if (buffer_len + section_len > buffer_capacity) {
                buffer_capacity = buffer_capacity + section_len;
                buffer = apprun_string_extend(buffer, buffer_capacity);
            }

            strcat(buffer, var_value);
            buffer_len += section_len;
        }

        itr = var_end;
    }


    return buffer;
}
