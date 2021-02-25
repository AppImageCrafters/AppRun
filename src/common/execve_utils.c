/**************************************************************************
 *
 * Copyright (c) 2021 Alexis Lopez Zubieta <contact@azubieta.net>
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
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "execve_utils.h"
#include "elf_utils.h"
#include "string_list.h"

char* apprun_resolve_runtime_interpreter(const char* exec_path) {
    char* interpreter_path = apprun_elf_read_pt_interp(exec_path);
    bool use_bundle_libc = getenv(APPRUN_USE_BUNDLE_LIBC) != NULL;

    if (use_bundle_libc) {
        char* old_interpreter_path = interpreter_path;
        char* path_parts[] = {getenv("APPDIR"), "opt/libc", interpreter_path, NULL};
        interpreter_path = apprun_string_list_join(path_parts, "/");

        free(old_interpreter_path);
    }
    return interpreter_path;
}

void apprun_exec_args_free(apprun_execve_args_t* args) {
    apprun_string_list_free(args->args);
    apprun_string_list_free(args->envp);
    free(args->file);
    free(args);
}

void apprun_print_exec_args(const char* filename, char* const* argv, char* const* envp) {
    fprintf(stderr, "  filename: \"%s\"\n", filename);
    fprintf(stderr, "  args: [ ");
    if (argv) {
        for (char* const* itr = argv; *itr != 0; itr++) {
            fprintf(stderr, "\"%s\"", *itr);
            if (*(itr + 1) != NULL)
                fprintf(stderr, ", ");
        }
    }

    fprintf(stderr, "]\n");

    fprintf(stderr, "  envp: [ \n");
    if (envp) {
        for (char* const* itr = envp; *itr != 0; itr++) {
            fprintf(stderr, "    \"%s\"", *itr);
            if (*(itr + 1) != NULL)
                fprintf(stderr, ", \n");
        }
    }


    fprintf(stderr, "]\n");
}