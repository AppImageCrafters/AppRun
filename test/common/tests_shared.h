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

#ifndef APPDIR_RUMTIME_TESTS_SHARED_H
#define APPDIR_RUMTIME_TESTS_SHARED_H

#include "hooks/environment.h"

void bailout();

#define assert_true(value) \
    if ( !(value) ) \
        bailout()

#define assert_false(value) \
    if ( (value) ) \
        bailout()

#define assert_eq(a, b) \
    if ( (a) != (b) ) \
        bailout()

void assert_str_eq(const char *str1, const char *str2);

void assert_str_list_eq(char *const *str_list_1, char *const *str_list_2);

void assert_command_succeed(int ret);

void assert_command_fails(int ret);

void set_private_env(char const *name, char const *value);

void unset_private_env(char const *name);

void print_string_list(char **string_list);

void assert_env_item_eq(apprun_env_item_t *a, apprun_env_item_t *b);

#endif //APPDIR_RUMTIME_TESTS_SHARED_H
