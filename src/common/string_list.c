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

#include "string_list.h"

char** apprun_extend_string_array(char** array, unsigned capacity) {
    char** new = calloc(capacity, sizeof(char*));

    char** orginal_itr = array;
    char** new_itr = new;

    for (; orginal_itr != NULL && *orginal_itr != NULL; orginal_itr++, new_itr++)
        *new_itr = *orginal_itr;

    free(array);
    return new;
}

char** adjust_string_array_size(char** array) {
    unsigned new_capacity = 1;
    for (char** itr = array; itr != NULL && *itr != NULL; itr++)
        new_capacity++;

    char** new = calloc(new_capacity, sizeof(char*));

    char** orginal_itr = array;
    char** new_itr = new;

    for (; orginal_itr != NULL && *orginal_itr != NULL; orginal_itr++, new_itr++)
        *new_itr = *orginal_itr;

    free(array);
    return new;
}

int apprun_string_list_len(char* const* x) {
    if (x == NULL)
        return 0;

    int len = 0;
    while (x[len] != 0)
        len++;

    return len;
}

void apprun_string_list_free(char** string_list) {
    if (string_list != NULL) {
        for (char** itr = string_list; *itr != NULL; itr++)
            free(*itr);

        free(string_list);
    }
}

char** apprun_string_list_alloc(unsigned int size) {
    char** ret = calloc(size, sizeof(char*));
    return ret;
}

int apprun_array_len(char* const* arr) {
    if (arr)
        return apprun_string_list_len(arr) + 1; // allocate extra space for the 0 termination
    else
        return 0;
}

char** apprun_string_list_dup(char* const* envp) {
    if (envp != NULL) {
        unsigned size = apprun_array_len(envp);
        char** copy = apprun_string_list_alloc(size);

        char* const* itr1 = envp;
        char** itr2 = copy;
        for (; *itr1 != NULL; itr1++, itr2++)
            *itr2 = strdup(*itr1);

        return copy;
    } else
        return NULL;
}

char* apprun_string_list_join(char* const* string_list, char* split) {
    unsigned string_list_len = apprun_string_list_len(string_list);
    unsigned split_len = strlen(split);
    unsigned str_size = 0;

    for (int i = 0; i < string_list_len; i++)
        str_size += strlen(string_list[i]) + split_len;

    char* str = calloc(str_size, sizeof(char));
    for (int i = 0; i < string_list_len; i++) {
        strcat(str, string_list[i]);
        if (i + 1 < string_list_len)
            strcat(str, split);
    }

    return str;
}
