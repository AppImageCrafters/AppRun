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

#include "../../src/hooks/environment.h"
#include "tests_shared.h"
#include "../../src/hooks/shared.h"


void test_env_item_is_changed() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

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
    fprintf(stdout, "Ok\n");
}

void test_env_item_unchanged_export_shared_item() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

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
    fprintf(stdout, "Ok\n");
}

void test_env_item_unchanged_export_external_item() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t external_item = {
            "DISPLAY",
            ":0",
            NULL,
            NULL
    };

    apprun_env_item_t* res = apprun_env_item_unchanged_export(&external_item);
    assert_env_item_eq(res, &external_item);
    apprun_env_item_free(res);
    fprintf(stdout, "Ok\n");
}

void test_env_item_unchanged_export_apprun_only_item() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t apprun_only_item = {
            "LD_PRELOAD",
            "libapprun_hooks.so",
            NULL,
            "libapprun_hooks.so"
    };
    apprun_env_item_t* res = apprun_env_item_unchanged_export(&apprun_only_item);
    assert_eq(res, NULL);

    fprintf(stdout, "Ok\n");
}

void test_env_item_unchanged_export_hidden_item() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

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
    fprintf(stdout, "Ok\n");
}

void test_env_item_changed_export_shared_item() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

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
    fprintf(stdout, "Ok\n");
}

void test_env_item_changed_export_external_item() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t external_item = {
            "DISPLAY",
            ":0",
            NULL,
            NULL
    };

    apprun_env_item_t* res = apprun_env_item_changed_export(&external_item);
    assert_env_item_eq(res, &external_item);
    apprun_env_item_free(res);
    fprintf(stdout, "Ok\n");
}

void test_env_item_changed_export_apprun_only_item() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t apprun_only_item = {
            "LD_PRELOAD",
            "libapprun_hooks.so",
            NULL,
            "libapprun_hooks.so"
    };
    apprun_env_item_t* res = apprun_env_item_changed_export(&apprun_only_item);
    assert_eq(res, NULL);

    fprintf(stdout, "Ok\n");
}

void test_env_item_changed_export_hidden_item() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

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
    fprintf(stdout, "Ok\n");
}

void test_env_item_export_apprun_only_item() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t apprun_only_item = {
            "LD_PRELOAD",
            "libapprun_hooks.so",
            NULL,
            "libapprun_hooks.so"
    };

    apprun_env_item_t* res = apprun_env_item_export(&apprun_only_item);
    assert_eq(res, NULL);

    fprintf(stdout, "Ok\n");
}

void test_env_item_export_empty_item() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t apprun_only_item = {"EMPTY", NULL, NULL, NULL};

    apprun_env_item_t* res = apprun_env_item_export(&apprun_only_item);
    assert_eq(res, NULL);

    fprintf(stdout, "Ok\n");
}

void test_env_item_list_from_envp() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

    char* envp[] = {
            "K1=V1",
            "APPRUN_ORIGINAL_K1=V0",
            "APPRUN_STARTUP_K1=V1",
            "K2=",
            "K3=0:1:2",
            "K4=",
            "APPRUN_ORIGINAL_K4=V0",
            "APPRUN_STARTUP_K4=",
            NULL,
    };

    apprun_env_item_list_t* res = apprun_env_item_list_from_envp(envp);

    apprun_env_item_t k1 = {"K1", "V1", "V0", "V1"};
    apprun_env_item_t k2 = {"K2", NULL, NULL, NULL};
    apprun_env_item_t k3 = {"K3", "0:1:2", NULL, NULL};
    apprun_env_item_t k4 = {"K4", NULL, "V0", NULL};

    assert_env_item_eq(res[0], &k1);
    assert_env_item_eq(res[1], &k2);
    assert_env_item_eq(res[2], &k3);
    assert_env_item_eq(res[3], &k4);

    apprun_env_item_list_free(res);


    fprintf(stdout, "Ok\n");
}

void test_export_env_item_list() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_t k1 = {"K1", "V1", "V0", "V1"};
    apprun_env_item_t k2 = {"K2", NULL, NULL, NULL};
    apprun_env_item_t k3 = {"K3", "0:1:2", NULL, NULL};
    apprun_env_item_t k4 = {"K4", NULL, "V0", NULL};
    apprun_env_item_list_t list[5] = {0x0};
    list[0] = &k1;
    list[1] = &k2;
    list[2] = &k3;
    list[3] = &k4;

    apprun_env_item_list_t* res = apprun_env_item_list_export((apprun_env_item_list_t const*) &list);

    apprun_env_item_t r0 = {"K1", "V0", NULL, NULL};
    apprun_env_item_t r1 = {"K3", "0:1:2", NULL, NULL};
    apprun_env_item_t r2 = {"K4", "V0", NULL, NULL};

    assert_env_item_eq(res[0], &r0);
    assert_env_item_eq(res[1], &r1);
    assert_env_item_eq(res[2], &r2);
    assert_eq(res[3], NULL);

    apprun_env_item_list_free(res);

    fprintf(stdout, "Ok\n");
}


void test_env_item_list_to_envp() {
    fprintf(stdout, "%s: ", __PRETTY_FUNCTION__);

    apprun_env_item_list_t list[5] = {0x0};
    apprun_env_item_t k1 = {"K1", "V1", "V0", "V1"};
    list[0] = &k1;
    apprun_env_item_t k2 = {"K2", NULL, NULL, NULL};
    list[1] = &k2;
    apprun_env_item_t k3 = {"K3", "0:1:2", NULL, NULL};
    list[2] = &k3;
    apprun_env_item_t k4 = {"K4", NULL, "V0", NULL};
    list[3] = &k4;

    char** res = apprun_env_item_list_to_envp(list);

    char* expected[] = {
            "K1=V1",
            "APPRUN_ORIGINAL_K1=V0",
            "APPRUN_STARTUP_K1=V1",
            "K3=0:1:2",
            "APPRUN_ORIGINAL_K4=V0",
            NULL,
    };

    assert_str_list_eq(expected, res);
    apprun_string_list_free(res);


    fprintf(stdout, "Ok\n");
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
    test_env_item_export_empty_item();

    test_env_item_list_from_envp();
    test_export_env_item_list();
    test_env_item_list_to_envp();

    return 0;
}


