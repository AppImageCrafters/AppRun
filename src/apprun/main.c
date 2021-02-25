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
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER>>>>>>> fix: allow launching executables without setting the SYSTEM_INTERP
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <limits.h>
#include <sys/stat.h>

#include "common/string_list.h"
#include "common/shell_utils.h"
#include "common/file_utils.h"
#include "hooks/environment.h"

#include "runtime_environment.h"
#include "runtime_interpreter.h"

#define die(...)                                    \
    do {                                            \
        fprintf(stderr, "APPRUN ERROR: " __VA_ARGS__);     \
        exit(1);                                    \
    } while(0);

char* add_appdir_libc_prefix_to_path(const char* appdir, const char* path) {
    char* appdir_interpreter_path = calloc(sizeof(char), PATH_MAX);
    memset(appdir_interpreter_path, 0, PATH_MAX);

    appdir_interpreter_path = strcat(appdir_interpreter_path, appdir);
    appdir_interpreter_path = strcat(appdir_interpreter_path, "/opt/libc");
    appdir_interpreter_path = strcat(appdir_interpreter_path, path);

    return appdir_interpreter_path;
}

void export_binary(const char* filename) {
    char* appimage_uuid = getenv("APPIMAGE_UUID");
    if (appimage_uuid == NULL)
        die("Unable to export runtime binaries, missing APPIMAGE_UUID in runtime environment")

    char target_path[PATH_MAX] = {0x0};
    strcat(target_path, "/tmp/appimage-");
    strcat(target_path, appimage_uuid);
    strcat(target_path, "-");
    strcat(target_path, strrchr(filename, '/') + 1);

    if (access(target_path, F_OK) == -1) {
        apprun_file_copy(filename, target_path);

        // Copy permissions and ownership
        struct stat fst;
        stat(filename, &fst);
        chown(target_path, fst.st_uid, fst.st_gid);
        chmod(target_path, fst.st_mode);
    }
}

void export_binaries(char* binaries) {
    char* token = strtok(binaries, ":");
    while (token != NULL) {
        export_binary(token);
        token = strtok(NULL, ":");
    }
}

char* resolve_origin(const char* apprun_path) {
    char* origin_path_end = strrchr(apprun_path, '/');
    char* origin_path = strndup(apprun_path, origin_path_end - apprun_path);

    return origin_path;
}

char* resolve_appdir_path(const char* root_env_file_path) {
    char* appdir = getenv("APPDIR");
    if (appdir == NULL) {
        if (root_env_file_path != NULL) {
            char* idx = strrchr(root_env_file_path, '/');
            appdir = strndup(root_env_file_path, idx - root_env_file_path);
        } else {
            die("Could not resolve APPDIR\n");
        }
    }
    return appdir;
}

char* find_module_env_file(char* apprun_path) {
    const char apprun_env_extension[] = ".env";
    const unsigned long apprun_env_extension_len = strlen(apprun_env_extension);

    const unsigned long possible_path_len = strlen(apprun_path) + apprun_env_extension_len;
    char* possible_path = malloc(possible_path_len);
    memset(possible_path, 0, possible_path_len);

    strcat(possible_path, apprun_path);
    strncat(possible_path, apprun_env_extension, apprun_env_extension_len);

#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: Looking for %s file at: %s\n", apprun_env_extension, possible_path);
#endif
    if (access(possible_path, F_OK) == 0)
        return possible_path;
    else
        free(possible_path);


    return NULL;
}

char* resolve_apprun_path() {
    char* apprun_path = realpath("/proc/self/exe", NULL);

#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: APPRUN_PATH=%s\n", apprun_path);
#endif
    return apprun_path;
}

char* build_env_file_path(char* apprun_path, unsigned long i) {
    char env_file_name[] = ".env";
    unsigned long env_file_name_len = 4;

    unsigned possible_path_len = i + env_file_name_len + 2 /*ZERO TERMINATION*/;
    char* possible_path = calloc(possible_path_len, sizeof(char));
    memset(possible_path, 0, possible_path_len);
    strncat(possible_path, apprun_path, i + 1);
    strncat(possible_path, env_file_name, env_file_name_len);

    return possible_path;
}

char* find_legacy_env_file(char* apprun_path) {
    char* slash_idx = strrchr(apprun_path, '/');

    if (slash_idx != NULL) {
        char* possible_path = build_env_file_path(apprun_path, slash_idx - apprun_path);
#ifdef DEBUG
        fprintf(stderr, "APPRUN_DEBUG: Looking for .env file at: %s\n", possible_path);
#endif
        if (access(possible_path, F_OK) == 0) {
            return possible_path;
        } else {
            free(possible_path);
        }
    }

    return NULL;
}

void launch(char* interpreter_path) {
    char* exec_path = getenv("EXEC_PATH");
    char* exec_args = getenv("EXEC_ARGS");

    char** user_args = apprun_shell_split_arguments(exec_args);
    unsigned user_args_len = apprun_string_list_len(user_args);
    char** argv = calloc(user_args_len + 3, sizeof(char*));
    argv[0] = interpreter_path;
    argv[1] = exec_path;

    for (int i = 0; i < user_args_len; i++)
        argv[i + 2] = user_args[i];

#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: executing ");
    for (char** itr = argv; itr != NULL && *itr != NULL; itr++)
        fprintf(stderr, "\"%s\" ", *itr);
    fprintf(stderr, "\n");
#endif

    int ret = execv(interpreter_path, argv);
    fprintf(stderr, "APPRUN_ERROR: %s", strerror(errno));
}

void set_runtime_library_paths(bool use_system_glibc) {
    char* runtime_library_path = NULL;
    if (use_system_glibc == true)
        runtime_library_path = apprun_shell_expand_variables("$APPDIR_LIBRARY_PATH:$LD_LIBRARY_PATH", NULL);
    else
        runtime_library_path = apprun_shell_expand_variables(
                "$LIBC_LIBRARY_PATH:$APPDIR_LIBRARY_PATH:$LD_LIBRARY_PATH",
                NULL);

    apprun_env_set("LD_LIBRARY_PATH", runtime_library_path, getenv("LD_LIBRARY_PATH"), runtime_library_path);
}

int main(int argc, char* argv[]) {
    char* apprun_path = resolve_apprun_path();
    char* origin_path = resolve_origin(apprun_path);
    apprun_env_set("ORIGIN", origin_path, NULL, origin_path);

    char* module_env_file_path = find_module_env_file(apprun_path);
    if (module_env_file_path != NULL)
        apprun_load_env_file(module_env_file_path, argv);

    if (module_env_file_path == NULL) {
        char* legacy_env_file_path = find_legacy_env_file(apprun_path);
        char* appdir_path = resolve_appdir_path(legacy_env_file_path);

        apprun_env_set("APPDIR", appdir_path, NULL, appdir_path);
        apprun_load_env_file(legacy_env_file_path, argv);
    }


    const char* exec_path = require_environment("EXEC_PATH");
    const char* appdir = require_environment("APPDIR");

    char* system_interpreter_path = apprun_elf_read_pt_interp(exec_path);

    char* system_libc_path = resolve_libc_from_interpreter_path(system_interpreter_path);
    char* system_libc_version = apprun_elf_read_glibc_version(system_libc_path);
    char* appdir_libc_version = require_environment("APPDIR_LIBC_VERSION");

#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: interpreter \"%s\" \n", strrchr(system_interpreter_path, '/') + 1);
    fprintf(stderr, "APPRUN_DEBUG: system glibc(%s), appdir glibc(%s) \n", system_libc_version,
            appdir_libc_version);
#endif

    bool use_system_glibc = apprun_compare_version_strings(system_libc_version, appdir_libc_version) >= 0;

    set_runtime_library_paths(use_system_glibc);

    char* runtime_interpreter_path = NULL;
    if (use_system_glibc == true)
        runtime_interpreter_path = system_interpreter_path;
    else
        runtime_interpreter_path = add_appdir_libc_prefix_to_path(appdir, system_interpreter_path);




    char* exported_binaries = getenv("EXPORTED_BINARIES");
    if (exported_binaries != NULL)
        export_binaries(exported_binaries);

    launch(runtime_interpreter_path);

    return 1;
}

