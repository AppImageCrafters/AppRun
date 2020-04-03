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

char const* const APPDIR_RUNTIME_ENV_ORIG_PREFIX = "APPIMAGE_ORIGINAL_";
char const* const APPDIR_RUNTIME_ENV_STARTUP_PREFIX = "APPIMAGE_STARTUP_";
char const* const APPDIR_RUNTIME_ENV = "APPDIR";

typedef struct {
    char** names;
    char** values;
} appdir_runtime_environment_t;

appdir_runtime_environment_t appdir_runtime_environment_alloc(size_t envc) {
    appdir_runtime_environment_t env;
    env.names = calloc(envc + 1, sizeof(char*));
    env.values = calloc(envc + 1, sizeof(char*));
    return env;
}

int appdir_runtime_environment_len(const appdir_runtime_environment_t env) {
    return appdir_runtime_string_list_len(env.names);
}

void appdir_runtime_environment_free(appdir_runtime_environment_t env) {
    appdir_runtime_string_list_free(env.names);
    appdir_runtime_string_list_free(env.values);
}

void
appdir_runtime_environment_append_item(appdir_runtime_environment_t env, char* name, unsigned long name_size, char* val,
                                       unsigned long val_size) {
    int count = appdir_runtime_environment_len(env);
    env.names[count] = calloc(name_size + 1, sizeof(char));
    env.values[count] = calloc(val_size + 1, sizeof(char));
    strncpy(env.names[count], name, name_size);
    strncpy(env.values[count], val, val_size);
}

int appdir_runtime_environment_find_name(appdir_runtime_environment_t env, char* name, unsigned long name_size) {
    int count = appdir_runtime_environment_len(env);
    for (int i = 0; i < count; i++) {
        if (!strncmp(env.names[i], name, name_size)) {
            return i;
        }
    }
    return -1;
}

char** appdir_runtime_environment_to_stringlist(appdir_runtime_environment_t env) {
    int len = appdir_runtime_environment_len(env);
    char** ret = appdir_runtime_string_list_alloc(len + 1);
    for (int i = 0; i < len; i++) {
        char* name = env.names[i];
        char* value = env.values[i];
        unsigned int result_len = strlen(name) + strlen(value) + 1;
        ret[i] = calloc(result_len + 1, sizeof(char));
        strcat(ret[i], name);
        strcat(ret[i], "=");
        strcat(ret[i], value);
    }
    return ret;
}

char** appdir_runtime_adjusted_environment(const char* filename, char* const* envp) {
    if (!envp) {
        return NULL;
    }

    int envc = appdir_runtime_string_list_len(envp);

    char* appdir = NULL;

    appdir_runtime_environment_t orig = appdir_runtime_environment_alloc(envc);
    appdir_runtime_environment_t startup = appdir_runtime_environment_alloc(envc);
    unsigned long orig_prefix_len = strlen(APPDIR_RUNTIME_ENV_ORIG_PREFIX);
    unsigned long startup_prefix_len = strlen(APPDIR_RUNTIME_ENV_STARTUP_PREFIX);
    for (int i = 0; i < envc; i++) {
        char* line = envp[i];
        long name_size = strchr(line, '=') - line;
        unsigned long val_size = strlen(line) - name_size - 1;

        if (!strncmp(line, APPDIR_RUNTIME_ENV_ORIG_PREFIX, orig_prefix_len)) {
            appdir_runtime_environment_append_item(orig, line + orig_prefix_len, name_size - orig_prefix_len,
                                                   line + name_size + 1, val_size);
        }
        if (!strncmp(line, APPDIR_RUNTIME_ENV_STARTUP_PREFIX, startup_prefix_len)) {
            appdir_runtime_environment_append_item(startup, line + startup_prefix_len, name_size - startup_prefix_len,
                                                   line + name_size + 1, val_size);
        }
        if (!strncmp(line, APPDIR_RUNTIME_ENV, strlen(APPDIR_RUNTIME_ENV))) {
            appdir = calloc(val_size + 1, sizeof(char));
            strncpy(appdir, line + name_size + 1, val_size);
        }
    }

    appdir_runtime_environment_t new_env = appdir_runtime_environment_alloc(envc);
    if (appdir && strncmp(filename, appdir, strlen(appdir)) != 0) {
        // we have a value for $APPDIR and are leaving it -- perform replacement
        for (int i = 0; i < envc; i++) {
            char* line = envp[i];
            if (!strncmp(line, APPDIR_RUNTIME_ENV_ORIG_PREFIX, strlen(APPDIR_RUNTIME_ENV_ORIG_PREFIX)) ||
                !strncmp(line, APPDIR_RUNTIME_ENV_STARTUP_PREFIX, strlen(APPDIR_RUNTIME_ENV_STARTUP_PREFIX))) {
                // we are not interested in the backup vars here, don't copy them over
                continue;
            }

            long name_size = strchr(line, '=') - line;
            unsigned long val_size = strlen(line) - name_size - 1;
            char* value = line + name_size + 1;
            unsigned long value_len = strlen(value);

            int at_startup = appdir_runtime_environment_find_name(startup, line, name_size);
            int at_original = appdir_runtime_environment_find_name(orig, line, name_size);
            if (at_startup == -1 || at_original == -1) {
                // no information, just keep it
                appdir_runtime_environment_append_item(new_env, line, name_size, value, value_len);
                continue;
            }

            char* at_start = startup.values[at_startup];
            unsigned long at_start_len = strlen(at_start);
            char* at_orig = orig.values[at_original];
            unsigned long at_orig_len = strlen(at_orig);

            // TODO HACK: do not copy over empty vars
            if (strlen(at_orig) == 0) {
                continue;
            }

            if (!strncmp(line + name_size + 1, startup.values[at_startup], val_size)) {
                // nothing changed since startup, restore old value
                appdir_runtime_environment_append_item(new_env, line, name_size, at_orig, at_orig_len);
                continue;
            }

            int chars_added = value_len > at_start_len;
            char* use_value = NULL;
            if (chars_added > 0) {
                // something was added to the current value
                // take _original_ value of the env var and append/prepend the same thing
                use_value = calloc(strlen(at_orig) + chars_added + 1, sizeof(char));
                if (!strncmp(value, at_start, at_start_len)) {
                    // append case
                    strcat(use_value, value);
                    strcat(use_value, at_orig + strlen(value));
                } else if (!strncmp(value + (value_len - at_start_len), at_start, at_start_len)) {
                    // prepend case
                    strcat(use_value, at_orig + strlen(value));
                    strcat(use_value, value);
                } else {
                    // none of the above methods matched
                    // assume the value changed completely and simply keep what the application set
                    free(use_value);
                    use_value = NULL;
                }
            }
            if (!use_value) {
                appdir_runtime_environment_append_item(new_env, line, name_size, value, value_len);
            } else {
                appdir_runtime_environment_append_item(new_env, line, name_size, use_value, strlen(use_value));
                free(use_value);
            }
        }
    }

    char** ret = NULL;
    if (appdir_runtime_environment_len(new_env) > 0) {
        ret = appdir_runtime_environment_to_stringlist(new_env);
    } else {
        // nothing changed
        ret = appdir_runtime_string_list_alloc(envc + 1);
        for (int i = 0; i < envc; i++) {
            unsigned long len = strlen(envp[i]);
            ret[i] = calloc(len + 1, sizeof(char));
            strncpy(ret[i], envp[i], len);
        }
    }
    appdir_runtime_environment_free(orig);
    appdir_runtime_environment_free(startup);
    appdir_runtime_environment_free(new_env);
    free(appdir);
    return ret;
}

apprun_env_item_t* apprun_env_item_unchanged_export(apprun_env_item_t const* item) {
    if (item->original_value != NULL) {
        apprun_env_item_t* copy = calloc(1, sizeof(apprun_env_item_t));
        copy->name = strdup(item->name);
        copy->current_value = strdup(item->original_value);

        return copy;
    }

    if (item->original_value == NULL && item->startup_value == NULL) {
        // this is item should not be tracked nor modified

        apprun_env_item_t* copy = calloc(1, sizeof(apprun_env_item_t));
        copy->name = strdup(item->name);
        copy->current_value = strdup(item->current_value);

        return copy;
    }

    return NULL;
}

apprun_env_item_t* apprun_env_item_create(char* name, char* current_vale, char* original_value, char* statup_value) {
    apprun_env_item_t* item = calloc(1, sizeof(apprun_env_item_t));
    if (name != NULL)
        item->name = strdup(name);

    if (current_vale != NULL)
        item->current_value = strdup(current_vale);

    if (original_value != NULL)
        item->original_value = strdup(original_value);

    if (statup_value != NULL)
        item->startup_value = strdup(statup_value);

    return item;
}

void apprun_env_item_free(apprun_env_item_t* item) {
    if (item == NULL)
        return;

    if (item->name != NULL)
        free(item->name);

    if (item->current_value != NULL)
        free(item->current_value);

    if (item->original_value != NULL)
        free(item->original_value);

    if (item->startup_value != NULL)
        free(item->startup_value);

    free(item);
}

bool apprun_env_item_is_changed(apprun_env_item_t const* item) {
    if (item->startup_value == item->current_value)
        return false;

    return !(item->startup_value != NULL &&
             item->current_value != NULL &&
             strcmp(item->startup_value, item->current_value) == 0);
}

char* apprun_env_replace_startup_by_original_section(const apprun_env_item_t* item,
                                                     const char* startup_value_section) {
    // replace startup_value by original_value
    unsigned new_value_len = strlen(item->original_value) +
                             strlen(item->current_value) - strlen(item->startup_value) + 1;
    char* new_value = calloc(new_value_len, sizeof(char));

    // copy prefixed values
    strncpy(new_value, item->current_value, startup_value_section - item->current_value);

    // add original value
    strcat(new_value, item->original_value);

    // add postfix
    strcat(new_value, startup_value_section + strlen(item->startup_value));
    return new_value;
}

apprun_env_item_t* apprun_env_item_changed_export(apprun_env_item_t* item) {
    if (item->original_value == NULL && item->startup_value == NULL) {
        // this is item should not be tracked nor modified

        apprun_env_item_t* copy = calloc(1, sizeof(apprun_env_item_t));
        copy->name = strdup(item->name);
        copy->current_value = strdup(item->current_value);

        return copy;
    }

    if (item->original_value == NULL)
        return NULL;

    // check if the startup_value was extended (pre or post fixed)
    char* startup_value_section = NULL;

    if (item->current_value != NULL && item->startup_value != NULL)
        startup_value_section = strstr(item->current_value, item->startup_value);

    if (startup_value_section != NULL) {
        char* new_value = apprun_env_replace_startup_by_original_section(item, startup_value_section);

        apprun_env_item_t* copy = calloc(1, sizeof(apprun_env_item_t));
        copy->name = strdup(item->name);
        copy->current_value = new_value;

        return copy;
    } else {
        // keep the current value and remove the APPRUN related vars
        apprun_env_item_t* copy = calloc(1, sizeof(apprun_env_item_t));
        copy->name = strdup(item->name);
        copy->current_value = strdup(item->current_value);

        return copy;
    }
}

bool apprun_env_item_is_tracked(const apprun_env_item_t* item) {
    return item->original_value != NULL || item->startup_value != NULL;
}

apprun_env_item_t* apprun_env_item_export(apprun_env_item_t* item) {
    if (!apprun_env_item_is_tracked(item)) {
        if (item->current_value == NULL)
            return NULL;

        apprun_env_item_t* copy = calloc(1, sizeof(apprun_env_item_t));
        copy->name = strdup(item->name);
        copy->current_value = strdup(item->current_value);

        return copy;
    }

    if (apprun_env_item_is_changed(item))
        return apprun_env_item_changed_export(item);
    else
        return apprun_env_item_unchanged_export(item);
}

char* apprun_env_str_entry_extract_name(char* string) {
    if (string) {
        char* sep = strstr(string, "=");
        return strndup(string, sep - string);
    }

    return NULL;
}

char* apprun_env_str_entry_extract_value(char* string) {
    if (string) {
        unsigned string_len = strlen(string);
        char* sep = strstr(string, "=");
        unsigned value_len = string_len - (sep - string);

        // assume empty string value as NULL
        if (value_len > 1)
            return strndup(sep + 1, value_len);
    }

    return NULL;
}

apprun_env_item_t* apprun_env_item_list_find(apprun_env_item_list_t* list, unsigned list_size, char* name) {
    for (unsigned i = 0; i < list_size; i++)
        if (strcmp(name, list[i]->name) == 0)
            return list[i];

    return NULL;
}

apprun_env_item_list_t* apprun_env_envp_to_env_item_list(char* const* envp) {
    unsigned env_origin_prefix_len = strlen(APPDIR_RUNTIME_ENV_ORIG_PREFIX);
    unsigned env_startup_prefix_len = strlen(APPDIR_RUNTIME_ENV_STARTUP_PREFIX);

    unsigned items_count = 0;

    unsigned envp_size = appdir_runtime_array_len(envp);
    apprun_env_item_list_t* list = calloc(envp_size, sizeof(apprun_env_item_t*));

    for (char* const* itr = envp; *itr != NULL; itr++) {
        char* prefixed_str = *itr;
        unsigned prefixed_str_len = strlen(prefixed_str);

        char* str = NULL;
        bool is_original_value = false;
        bool is_startup_value = false;

        if (strncmp(APPDIR_RUNTIME_ENV_ORIG_PREFIX, prefixed_str, env_origin_prefix_len) == 0) {
            str = strndup(prefixed_str + env_origin_prefix_len, prefixed_str_len - env_origin_prefix_len);
            is_original_value = true;
        }

        if (strncmp(APPDIR_RUNTIME_ENV_STARTUP_PREFIX, prefixed_str, env_startup_prefix_len) == 0) {
            str = strndup(prefixed_str + env_startup_prefix_len, prefixed_str_len - env_startup_prefix_len);
            is_startup_value = true;
        }

        if (str == NULL)
            str = strdup(prefixed_str);

        char* name = apprun_env_str_entry_extract_name(str);
        char* value = apprun_env_str_entry_extract_value(str);
        free(str);

        apprun_env_item_t* item = apprun_env_item_list_find(list, items_count, name);
        if (item == NULL) {
            item = calloc(1, sizeof(apprun_env_item_t));
            item->name = name;

            list[items_count] = item;
            items_count++;
        } else {
            // it's safe to free the name as it's not going to be used
            free(name);
        }

        if (is_original_value)
            item->original_value = value;

        if (is_startup_value)
            item->startup_value = value;

        if (!is_original_value && !is_startup_value)
            item->current_value = value;
    }

    return list;
}

void apprun_env_item_list_free(apprun_env_item_list_t* list) {
    for (apprun_env_item_list_t* itr = list; *itr != NULL; itr++)
        apprun_env_item_free(*itr);

    free(list);
}

apprun_env_item_list_t* apprun_env_item_list_export(apprun_env_item_list_t const* list) {
    unsigned new_list_size = apprun_env_item_list_size(list) + 1;
    unsigned new_list_item_count = 0;
    apprun_env_item_list_t* new_list = calloc(new_list_size, sizeof(apprun_env_item_list_t*));

    for (apprun_env_item_list_t const* itr = list; *itr != NULL; itr++) {
        apprun_env_item_t* item = apprun_env_item_export(*itr);
        if (item != NULL) {
            new_list[new_list_item_count] = item;
            new_list_item_count++;
        }
    }

    return new_list;
}

unsigned apprun_env_item_list_size(apprun_env_item_list_t const* list) {
    unsigned size = 0;
    while (list[size] != NULL)
        size++;

    return size;
}
