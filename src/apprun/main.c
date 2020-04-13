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
#include <unistd.h>
#include "common/string_list.h"
#include "common/shell_utils.h"

#include "runtime_environmet.h"
#include "runtime_interpreter.h"

#define die(...)                                    \
    do {                                            \
        fprintf(stderr, "APPRUN ERROR: " __VA_ARGS__);     \
        exit(1);                                    \
    } while(0);

char* get_appdir() {
    char* appdir = getenv("APPDIR");

    if (appdir == NULL)
        appdir = dirname(realpath("/proc/self/exe", NULL));

    return appdir;
}


void launch() {
    char* exec_path = getenv("EXEC_PATH");
    char* exec_args = getenv("EXEC_ARGS");

    char** user_args = apprun_shell_split_arguments(exec_args);
    unsigned user_args_len = apprun_string_list_len(user_args);
    char** argv = calloc(user_args_len + 2, sizeof(char*));
    argv[0] = exec_path;
    for (int i = 0; i < user_args_len; i++)
        argv[i + 1] = user_args[i];

#ifdef DEBUG
    char* debug = apprun_string_list_join(argv, " ");
    fprintf(stderr, "APPRUN_DEBUG: %s\n", debug);
#endif
    execv(exec_path, argv);
}


int main(int argc, char* argv[]) {
    char* appdir = get_appdir();
    if (!appdir)
        die("Could not resolve APPDIR\n");

    setup_appdir(appdir);
    setup_runtime_environment(appdir, argv);
    setup_interpreter();

    launch();

    return 1;
}

