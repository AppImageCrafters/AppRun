/**************************************************************************
 *
 * Copyright (C) 2016 Sven Brauch <mail@svenbrauch.de>
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
#include <malloc.h>
#include <string.h>


#include "environment.h"
#include "shared.h"
char* APPIMAGE_ORIG_PREFIX = "APPIMAGE_ORIGINAL_";
char* APPIMAGE_STARTUP_PREFIX = "APPIMAGE_STARTUP_";
char* APPDIR = "APPDIR";

typedef struct {
    char** names;
    char** values;
} environment;

environment environment_alloc(size_t envc) {
    environment env;
    env.names = calloc(envc+1, sizeof(char*));
    env.values = calloc(envc+1, sizeof(char*));
    return env;
}

int arr_len(char* const x[]) {
    int len = 0;
    while ( x[len] != 0 ) {
        len++;
    }
    return len;
}

char** stringlist_alloc(int size) {
    char** ret = calloc(size, sizeof(char*));
    return ret;
}

int environment_len(const environment env) {
    return arr_len(env.names);
}

void environment_free(environment env) {
    appdir_runtime_string_list_free(env.names);
    appdir_runtime_string_list_free(env.values);
}

void environment_append_item(environment env, char* name, int name_size, char* val, int val_size) {
    int count = environment_len(env);
    env.names[count] = calloc(name_size+1, sizeof(char));
    env.values[count] = calloc(val_size+1, sizeof(char));
    strncpy(env.names[count], name, name_size);
    strncpy(env.values[count], val, val_size);
}

int environment_find_name(environment env, char* name, int name_size) {
    int count = environment_len(env);
    for ( int i = 0; i < count; i++ ) {
        if ( !strncmp(env.names[i], name, name_size) ) {
            return i;
        }
    }
    return -1;
}

char** environment_to_stringlist(environment env) {
    int len = environment_len(env);
    char** ret = stringlist_alloc(len+1);
    for ( int i = 0; i < len; i++ ) {
        char* name = env.names[i];
        char* value = env.values[i];
        int result_len = strlen(name) + strlen(value) + 1;
        ret[i] = calloc(result_len+1, sizeof(char));
        strcat(ret[i], name);
        strcat(ret[i], "=");
        strcat(ret[i], value);
    }
    return ret;
}

char** adjusted_environment(const char* filename, char* const envp[]) {
    if ( !envp ) {
        return NULL;
    }

    int envc = arr_len(envp);

    char* appdir = NULL;

    environment orig = environment_alloc(envc);
    environment startup = environment_alloc(envc);
    int orig_prefix_len = strlen(APPIMAGE_ORIG_PREFIX);
    int startup_prefix_len = strlen(APPIMAGE_STARTUP_PREFIX);
    for ( int i = 0; i < envc; i++ ) {
        char* line = envp[i];
        int name_size = strchr(line, '=')-line;
        int val_size = strlen(line)-name_size-1;

        if ( !strncmp(line, APPIMAGE_ORIG_PREFIX, orig_prefix_len) ) {
            environment_append_item(orig, line+orig_prefix_len, name_size-orig_prefix_len,
                                    line+name_size+1, val_size);
        }
        if ( !strncmp(line, APPIMAGE_STARTUP_PREFIX, startup_prefix_len) ) {
            environment_append_item(startup, line+startup_prefix_len, name_size-startup_prefix_len,
                                    line+name_size+1, val_size);
        }
        if ( !strncmp(line, APPDIR, strlen(APPDIR)) ) {
            appdir = calloc(val_size+1, sizeof(char));
            strncpy(appdir, line+name_size+1, val_size);
        }
    }

    environment new_env = environment_alloc(envc);
    if ( appdir && strncmp(filename, appdir, strlen(appdir)) ) {
        // we have a value for $APPDIR and are leaving it -- perform replacement
        for ( int i = 0; i < envc; i++ ) {
            char* line = envp[i];
            if ( !strncmp(line, APPIMAGE_ORIG_PREFIX, strlen(APPIMAGE_ORIG_PREFIX)) ||
                 !strncmp(line, APPIMAGE_STARTUP_PREFIX, strlen(APPIMAGE_STARTUP_PREFIX)) )
            {
                // we are not interested in the backup vars here, don't copy them over
                continue;
            }

            int name_size = strchr(line, '=')-line;
            int val_size = strlen(line)-name_size-1;
            char* value = line+name_size+1;
            int value_len = strlen(value);

            int at_startup = environment_find_name(startup, line, name_size);
            int at_original = environment_find_name(orig, line, name_size);
            if ( at_startup == -1 || at_original == -1 ) {
                // no information, just keep it
                environment_append_item(new_env, line, name_size, value, value_len);
                continue;
            }

            char* at_start = startup.values[at_startup];
            int at_start_len = strlen(at_start);
            char* at_orig = orig.values[at_original];
            int at_orig_len = strlen(at_orig);

            // TODO HACK: do not copy over empty vars
            if ( strlen(at_orig) == 0 ) {
                continue;
            }

            if ( !strncmp(line+name_size+1, startup.values[at_startup], val_size) ) {
                // nothing changed since startup, restore old value
                environment_append_item(new_env, line, name_size, at_orig, at_orig_len);
                continue;
            }

            int chars_added = value_len > at_start_len;
            char* use_value = NULL;
            if ( chars_added > 0 ) {
                // something was added to the current value
                // take _original_ value of the env var and append/prepend the same thing
                use_value = calloc(strlen(at_orig) + chars_added + 1, sizeof(char));
                if ( !strncmp(value, at_start, at_start_len) ) {
                    // append case
                    strcat(use_value, value);
                    strcat(use_value, at_orig + strlen(value));
                }
                else if ( !strncmp(value+(value_len-at_start_len), at_start, at_start_len) ) {
                    // prepend case
                    strcat(use_value, at_orig + strlen(value));
                    strcat(use_value, value);
                }
                else {
                    // none of the above methods matched
                    // assume the value changed completely and simply keep what the application set
                    free(use_value);
                    use_value = NULL;
                }
            }
            if ( !use_value ) {
                environment_append_item(new_env, line, name_size, value, value_len);
            }
            else {
                environment_append_item(new_env, line, name_size, use_value, strlen(use_value));
                free(use_value);
            }
        }
    }

    char** ret = NULL;
    if ( environment_len(new_env) > 0 ) {
        ret = environment_to_stringlist(new_env);
    }
    else {
        // nothing changed
        ret = stringlist_alloc(envc+1);
        for ( int i = 0; i < envc; i++ ) {
            int len = strlen(envp[i]);
            ret[i] = calloc(len+1, sizeof(char));
            strncpy(ret[i], envp[i], len);
        }
    }
    environment_free(orig);
    environment_free(startup);
    environment_free(new_env);
    free(appdir);
    return ret;
}