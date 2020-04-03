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

#ifndef APPDIR_RUMTIME_ENVIRONMENT_H
#define APPDIR_RUMTIME_ENVIRONMENT_H

#include <stdbool.h>

extern char const* const APPDIR_RUNTIME_ENV_ORIG_PREFIX;
extern char const* const APPDIR_RUNTIME_ENV_STARTUP_PREFIX;
extern char const* const APPDIR_RUNTIME_ENV;

typedef struct {
    char* name;
    char* current_value;
    char* original_value;
    char* startup_value;
} apprun_env_item_t;

typedef apprun_env_item_t* apprun_env_item_list_t;

apprun_env_item_t* apprun_env_item_create(char* name, char* current_vale, char* original_value, char* statup_value);

void apprun_env_item_free(apprun_env_item_t* item);

void apprun_env_item_list_free(apprun_env_item_list_t* list);

apprun_env_item_list_t* apprun_env_item_list_export(apprun_env_item_list_t const* list);

unsigned apprun_env_item_list_size(apprun_env_item_list_t const* list);

char** appdir_runtime_adjusted_environment(const char* filename, char* const* envp);

bool apprun_env_item_is_changed(apprun_env_item_t const* item);

apprun_env_item_t* apprun_env_item_export(apprun_env_item_t* item);

apprun_env_item_t* apprun_env_item_changed_export(apprun_env_item_t* item);

apprun_env_item_t* apprun_env_item_unchanged_export(apprun_env_item_t const* item);

apprun_env_item_list_t* apprun_env_item_list_from_envp(char* const* envp);

char** apprun_env_item_list_to_envp(apprun_env_item_list_t* list);

#endif //APPDIR_RUMTIME_ENVIRONMENT_H
