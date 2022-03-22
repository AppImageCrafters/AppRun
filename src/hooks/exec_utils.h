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

#ifndef APPDIR_RUMTIME_SHARED_H
#define APPDIR_RUMTIME_SHARED_H

#include <stdbool.h>

#define APPRUN_ENV_APPDIR "APPDIR"
#define APPRUN_ENV_INTERPRETER "INTERPRETER"

typedef struct {
    char *file;
    char **args;
    char **envp;
} apprun_exec_args_t;

void apprun_exec_args_free(apprun_exec_args_t *args);

void apprun_print_exec_args(const char *filename, char *const *argv, char *const *envp);

apprun_exec_args_t *apprun_duplicate_exec_args(const char *filename, char *const *argv);

char **apprun_set_original_workdir_env(char *const *envp);

void chdir_to_runtime();

apprun_exec_args_t *apprun_adjusted_exec_args(const char *filename, char *const *argv, char *const *envp);

/**
 * Read shebang from file
 *
 * @param fd
 * @return shebang without '#! ' chars (all spaces on left side are removed). Or NULL.
 */
char * apprun_read_shebang(const char *filename);
char *apprun_parse_shebang(char *buf, size_t br);

/**
 * Extract the interpreter path from shebang line.
 *
 * @param shebang
 * @return interpreter path or NULL
 */
char *apprun_shebang_extract_interpreter_path(const char *shebang);

/**
 * Checks if the interpreter path specified in the shebang is relative to the appdir
 * @param shebang
 * @param appdir
 * @return true if the executable requires an interpreter and if this is relative to the appdir otherwise returns false.
 */
bool apprun_shebang_requires_external_executable(const char *shebang, const char *appdir);

#endif //APPDIR_RUMTIME_SHARED_H
