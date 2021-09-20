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

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define BT_BUF_SIZE 100

char* find_hooked_symbol() {
    int nptrs;
    void* buffer[BT_BUF_SIZE];
    char** strings;

    nptrs = backtrace(buffer, BT_BUF_SIZE);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }
    char* apprun_hooks_entry;
    for (int j = 0; j < nptrs; j++) {
        if (strstr(strings[j], "libapprun_hooks.so") != NULL) {
            apprun_hooks_entry = strings[j];
        }
    }

    char* symbol_name_being = strstr(apprun_hooks_entry, "(");

    // symbol name lookup is not perfect, in case of failure just return UNKNOWN
    if (symbol_name_being == NULL)
        return strdup("UNKNOWN");

    char* symbol_name_end = strstr(symbol_name_being, "+");
    char* symbol_name = strndup(symbol_name_being + 1, symbol_name_end - symbol_name_being - 1);

    free(strings);
    return symbol_name;
}
