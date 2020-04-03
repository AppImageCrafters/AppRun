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

#include "../src/environment.h"
#include "tests_shared.h"


void test_env_item_is_changed() {
    fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t not_changed_item = {
            "LD_PRELOAD",
            "libapprun_hooks.so",
            NULL,
            "libapprun_hooks.so"
    };
    assert_false(apprun_env_item_is_changed(&not_changed_item));

    apprun_env_item_t changed_item = {
            "HOME",
            "/home/that",
            NULL,
            "/home/this"
    };

    assert_true(apprun_env_item_is_changed(&changed_item));
    fprintf(stderr, "Ok\n");
}

void test_env_item_unchanged_export_shared_item() {
    fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t shared_item = {
            "LD_LIBRARY_PATH",
            "/tmp/app/lib:/lib",
            "/lib",
            "/tmp/app/lib:/lib"
    };

    apprun_env_item_t* res = apprun_env_item_unchanged_export(&shared_item);

    apprun_env_item_t expected = {
            "LD_LIBRARY_PATH",
            "/lib",
            NULL,
            NULL
    };

    assert_env_item_eq(res, &expected);

    apprun_env_item_free(res);
    fprintf(stderr, "Ok\n");
}

void test_env_item_unchanged_export_external_item() {
    fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t external_item = {
            "DISPLAY",
            ":0",
            NULL,
            NULL
    };

    apprun_env_item_t* res = apprun_env_item_unchanged_export(&external_item);
    assert_env_item_eq(res, &external_item);
    apprun_env_item_free(res);
    fprintf(stderr, "Ok\n");
}

void test_env_item_unchanged_export_apprun_only_item() {
    fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t apprun_only_item = {
            "LD_PRELOAD",
            "libapprun_hooks.so",
            NULL,
            "libapprun_hooks.so"
    };
    apprun_env_item_t* res = apprun_env_item_unchanged_export(&apprun_only_item);
    assert_eq(res, NULL);

    fprintf(stderr, "Ok\n");
}

void test_env_item_unchanged_export_hidden_item() {
    fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t shared_item = {
            "LC_ALL",
            NULL,
            "C",
            NULL
    };

    apprun_env_item_t* res = apprun_env_item_unchanged_export(&shared_item);

    apprun_env_item_t expected = {
            "LC_ALL",
            "C",
            NULL,
            NULL
    };

    assert_env_item_eq(res, &expected);

    apprun_env_item_free(res);
    fprintf(stderr, "Ok\n");
}

void test_env_item_changed_export_shared_item() {
    fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t shared_item = {
            "PATH",
            "/sbin:/tmp/app/bin:/bin:/usr/bin",
            "/bin",
            "/tmp/app/bin:/bin"
    };

    apprun_env_item_t* res = apprun_env_item_changed_export(&shared_item);

    apprun_env_item_t expected = {
            "PATH",
            "/sbin:/bin:/usr/bin",
            NULL,
            NULL
    };

    assert_env_item_eq(res, &expected);

    apprun_env_item_free(res);
    fprintf(stderr, "Ok\n");
}

void test_env_item_changed_export_external_item() {
    fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t external_item = {
            "DISPLAY",
            ":0",
            NULL,
            NULL
    };

    apprun_env_item_t* res = apprun_env_item_changed_export(&external_item);
    assert_env_item_eq(res, &external_item);
    apprun_env_item_free(res);
    fprintf(stderr, "Ok\n");
}

void test_env_item_changed_export_apprun_only_item() {
    fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t apprun_only_item = {
            "LD_PRELOAD",
            "libapprun_hooks.so",
            NULL,
            "libapprun_hooks.so"
    };
    apprun_env_item_t* res = apprun_env_item_changed_export(&apprun_only_item);
    assert_eq(res, NULL);

    fprintf(stderr, "Ok\n");
}

void test_env_item_changed_export_hidden_item() {
    fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t shared_item = {
            "LC_ALL",
            "es_CU.UTF-8",
            "C",
            NULL
    };

    apprun_env_item_t* res = apprun_env_item_changed_export(&shared_item);

    apprun_env_item_t expected = {
            "LC_ALL",
            "es_CU.UTF-8",
            NULL,
            NULL
    };

    assert_env_item_eq(res, &expected);

    apprun_env_item_free(res);
    fprintf(stderr, "Ok\n");
}

void test_env_item_export_apprun_only_item() {
    fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t apprun_only_item = {
            "LD_PRELOAD",
            "libapprun_hooks.so",
            NULL,
            "libapprun_hooks.so"
    };

    apprun_env_item_t* res = apprun_env_item_export(&apprun_only_item);
    assert_eq(res, NULL);

    fprintf(stderr, "Ok\n");
}

int main(int argc, char** argv, char* envp[]) {
    test_env_item_is_changed();

    test_env_item_unchanged_export_shared_item();
    test_env_item_unchanged_export_external_item();
    test_env_item_unchanged_export_apprun_only_item();
    test_env_item_unchanged_export_hidden_item();

    test_env_item_changed_export_shared_item();
    test_env_item_changed_export_external_item();
    test_env_item_changed_export_apprun_only_item();
    test_env_item_changed_export_hidden_item();

    test_env_item_export_apprun_only_item();

    return 0;
}


