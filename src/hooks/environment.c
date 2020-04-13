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

#include "common/string_list.h"
#include "environment.h"

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
    if (string && strlen(string) > 0) {
        unsigned string_len = strlen(string);
        char* sep = strstr(string, "=");
        unsigned value_len = string_len - (sep - string);

        // assume empty string value as NULL
        if (value_len > 1)
            return strndup(sep + 1, value_len);
    }

    return NULL;
}

apprun_env_item_t* apprun_env_item_list_find(apprun_env_item_list_t* list, char* name) {
    for (apprun_env_item_list_t* itr = list; *itr != NULL; itr++) {
        apprun_env_item_t* item = *itr;
        if (strcmp(name, item->name) == 0)
            return item;
    }


    return NULL;
}

apprun_env_item_list_t* apprun_env_item_list_from_envp(char* const* envp) {
    unsigned env_origin_prefix_len = strlen(APPRUN_ENV_ORIG_PREFIX);
    unsigned env_startup_prefix_len = strlen(APPRUN_ENV_STARTUP_PREFIX);

    unsigned items_count = 0;

    unsigned envp_size = apprun_array_len(envp);
    apprun_env_item_list_t* list = calloc(envp_size, sizeof(apprun_env_item_t*));

    for (char* const* itr = envp; *itr != NULL; itr++) {
        char* prefixed_str = *itr;
        unsigned prefixed_str_len = strlen(prefixed_str);

        char* str = NULL;
        bool is_original_value = false;
        bool is_startup_value = false;

        if (strncmp(APPRUN_ENV_ORIG_PREFIX, prefixed_str, env_origin_prefix_len) == 0) {
            str = strndup(prefixed_str + env_origin_prefix_len, prefixed_str_len - env_origin_prefix_len);
            is_original_value = true;
        }

        if (strncmp(APPRUN_ENV_STARTUP_PREFIX, prefixed_str, env_startup_prefix_len) == 0) {
            str = strndup(prefixed_str + env_startup_prefix_len, prefixed_str_len - env_startup_prefix_len);
            is_startup_value = true;
        }

        if (str == NULL)
            str = strdup(prefixed_str);

        char* name = apprun_env_str_entry_extract_name(str);
        char* value = apprun_env_str_entry_extract_value(str);
        free(str);

        apprun_env_item_t* item = apprun_env_item_list_find(list, name);
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

unsigned int apprun_env_item_list_to_envp_len(apprun_env_item_list_t* list) {
    unsigned len = 0;
    for (apprun_env_item_list_t* itr = list; *itr != NULL; itr++) {
        apprun_env_item_t* item = *itr;
        if (item->current_value != NULL)
            len++;

        if (item->startup_value != NULL)
            len++;

        if (item->original_value != NULL)
            len++;
    }

    return len;
}

char* apprun_env_envp_entry_create(char const* name_prefix, char const* name, char const* value) {
    unsigned size = 0;
    if (name_prefix != NULL)
        size += strlen(name_prefix);
    if (name != NULL)
        size += strlen(name);
    if (value != NULL)
        size += strlen(value);

    // allocate enough space for '=' and the NULL termination
    char* entry = calloc(size + 2, sizeof(char));
    if (name_prefix)
        strcat(entry, name_prefix);

    if (name)
        strcat(entry, name);

    strcat(entry, "=");

    if (value)
        strcat(entry, value);

    return entry;
}

char** apprun_env_item_list_to_envp(apprun_env_item_list_t* list) {
    unsigned int envp_len = apprun_env_item_list_to_envp_len(list);

    // Allocate an extra space for NULL termination
    char** envp = calloc(envp_len + 1, sizeof(char*));
    unsigned count = 0;

    for (apprun_env_item_list_t* itr = list; *itr != NULL; itr++) {
        apprun_env_item_t* item = *itr;
        if (item->current_value != NULL) {
            char* env_entry = apprun_env_envp_entry_create(NULL, item->name, item->current_value);
            envp[count] = env_entry;
            count++;
        }

        if (item->original_value != NULL) {
            char* env_entry = apprun_env_envp_entry_create(APPRUN_ENV_ORIG_PREFIX, item->name,
                                                           item->original_value);
            envp[count] = env_entry;
            count++;
        }

        if (item->startup_value != NULL) {
            char* env_entry = apprun_env_envp_entry_create(APPRUN_ENV_STARTUP_PREFIX, item->name,
                                                           item->startup_value);
            envp[count] = env_entry;
            count++;
        }
    }

    return envp;
}

char** apprun_export_envp(char* const* envp) {
    if (envp == NULL)
        return NULL;

    apprun_env_item_list_t* orginial = apprun_env_item_list_from_envp(envp);
    apprun_env_item_list_t* exported = apprun_env_item_list_export(orginial);
    char** adjusted_envp = apprun_env_item_list_to_envp(exported);

    apprun_env_item_list_free(orginial);
    apprun_env_item_list_free(exported);

    return adjusted_envp;
}