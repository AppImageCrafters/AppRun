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
#include <stdlib.h>
#include <libgen.h>
#include <string.h>

#include "../hooks/environment.h"
#include "runtime_environmet.h"
#include "../common/file_utils.h"

#define die(...)                                    \
    do {                                            \
        fprintf(stderr, "APPRUN ERROR: " __VA_ARGS__);     \
        exit(1);                                    \
    } while(0);

char* get_appdir() {
    char* appdir = getenv("APPDIR");

    if (appdir == NULL)
        appdir = dirname(realpath("/proc/self/exe", NULL));

    if (!appdir)
        die("Could not resolve APPDIR\n");

    return appdir;
}



int main(int argc, char* argv[]) {
    char* appdir = get_appdir();

    setup_runtime_environment(appdir);

    return 0;
}


