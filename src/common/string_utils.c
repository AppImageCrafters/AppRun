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
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include "string_utils.h"

char *apprun_string_remove_trailing_new_line(const char *str) {
    if (str == NULL)
        return NULL;

    unsigned line_len = strlen(str);
    if (line_len > 0 && str[line_len - 1] == '\n')
        line_len--;

    char *new = calloc(line_len + 1, sizeof(char));
    memcpy(new, str, line_len);
    return new;
}

char *apprun_string_extend(char *string, unsigned int new_size) {
    char *new = calloc(new_size, sizeof(char *));
    strcpy(new, string);
    free(string);

    return new;
}

const char *apprun_string_consume_until(const char *itr, const char *delimiters) {
    while (itr != NULL && *itr != '\0' && strchr(delimiters, *itr) == NULL)
        itr++;

    return itr;
}

bool apprun_string_is_all_blanks(const char *str) {
    for (const char *itr = str; *itr != '\0'; itr++) {
        if (!isspace(*itr))
            return false;
    }

    return true;
}


char *apprun_string_trim(char *str) {
    char *useful_section_start = str;

    while (isspace(*useful_section_start))
        useful_section_start++;

    char *useful_section_end = useful_section_start;
    while (*useful_section_end != '\0')
        useful_section_end++;

    while (((useful_section_end - 1) > useful_section_start) && isspace(*(useful_section_end - 1)))
        useful_section_end--;

    char *result = strndup(useful_section_start, useful_section_end - useful_section_start);
    return result;
}

long compare_version_strings(const char *a, const char *b) {
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