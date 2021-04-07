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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "shell_utils.h"
#include "string_utils.h"
#include "string_list.h"

bool apprun_shell_is_var_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '@' || c == '_';
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


char* apprun_argv_to_env(char* const* string_list) {
    unsigned string_list_len = apprun_string_list_len(string_list);
    unsigned str_size = 0;

    // space required by two quotation marks and one space
    const unsigned extra_chars_len = 3;

    for (int i = 0; i < string_list_len; i++)
        str_size += strlen(string_list[i]) + extra_chars_len;

    char* str = calloc(str_size + 1, sizeof(char));
    memset(str, 0, str_size + 1);

    for (int i = 0; i < string_list_len; i++) {
        strcat(str, "\"");
        strcat(str, string_list[i]);
        strcat(str, "\"");
        if (i + 1 < string_list_len)
            strcat(str, " ");
    }

    return str;
}

char* apprun_shell_resolve_var_value(char* const* argv, const char* var_name) {
    unsigned argc = 0;
    if (argv)
        argc = apprun_string_list_len(argv);

    char* var_value = NULL;
    if (isdigit(*var_name)) {
        long idx = atol(var_name);
        if (idx <= argc)
            var_value = strdup(argv[idx]);
    }

    if (strcmp(var_name, "@") == 0 && argv != NULL)
        var_value = apprun_argv_to_env(argv + 1);

    if (isalpha(*var_name)) {
        var_value = getenv(var_name);
        if (var_value)
            var_value = strdup(var_value);
    }


    return var_value;
}

char* apprun_shell_expand_variables(char const* str, char** argv) {
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
            char* var_value = apprun_shell_resolve_var_value(argv, var_name);

            free(var_name);

            if (var_value) {
                unsigned section_len = strlen(var_value);
                if (buffer_len + section_len > buffer_capacity) {
                    buffer_capacity = buffer_capacity + section_len;
                    buffer = apprun_string_extend(buffer, buffer_capacity);
                }

                strcat(buffer, var_value);
                buffer_len += section_len;
                free(var_value);
            }
        }

        itr = var_end;
    }


    return buffer;
}

const char* apprun_string_consume_spaces(const char* itr) {
    while (itr != NULL && *itr != '\0' && isspace(*itr))
        itr++;

    return itr;
}

char** apprun_shell_split_arguments(char const* str) {
    if (str == NULL)
        return NULL;

    static const char single_quote_delimiter[] = "\'";
    static const char double_quote_delimiter[] = "\"";
    static const char regular_quote_delimiter[] = "\"\'\\ \t";

    char** splits = calloc(strlen(str) + 1, sizeof(char*));
    unsigned split_count = 0;

    const char* begin = str;
    const char* end = str;
    char buffer[1024] = {0x0};

    while (*begin != '\0') {
        begin = apprun_string_consume_spaces(begin);
        bool consumed = false;

        if (*begin == '\\') {
            strncat(buffer, begin + 1, 1);
            end = begin + 2;
            consumed = true;
        }

        if (!consumed && *begin == '\'') {
            end = apprun_string_consume_until(begin + 1, single_quote_delimiter);
            strncat(buffer, begin + 1, end - begin - 1);
            consumed = true;

            end++;
        }

        if (!consumed && *begin == '\"') {
            end = apprun_string_consume_until(begin + 1, double_quote_delimiter);
            strncat(buffer, begin + 1, end - begin - 1);
            consumed = true;

            end++;
        }

        if (!consumed && *begin != '\'' && *begin != '\"' && *begin != '\\') {
            end = apprun_string_consume_until(begin, regular_quote_delimiter);
            strncat(buffer, begin, end - begin);
        }

        if (isspace(*end) || *end == 0) {
            if (!apprun_string_is_all_blanks(buffer)) {
                splits[split_count] = apprun_string_trim(buffer);
                split_count++;
            }

            memset(buffer, 0, 1024);
        }

        begin = end;
    }

    return splits;
}
