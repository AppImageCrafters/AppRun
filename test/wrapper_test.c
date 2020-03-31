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
#include <stdlib.h>
#include <unistd.h>

#include "tests_shared.h"

void setup_appdir(const char* appdir_path) {
    setenv("APPDIR", appdir_path, 1);
    setenv("APPIMAGE_ORIGINAL_APPDIR", "", 1);
    setenv("APPIMAGE_STARTUP_APPDIR", appdir_path, 1);
}

void test_restore_original_env_for_external_binaries() {
    fprintf(stderr, "Test restore original environment when calling external binaries: ");

    setup_appdir("/tmp");

    // grep fails if nothing matches
    assert_command_fails(system("/usr/bin/printenv APPDIR >> /dev/null"));
    assert_command_fails(system("/usr/bin/printenv LD_PRELOAD >> /dev/null"));

    fprintf(stderr, "Ok\n");
}

void test_keep_appdir_env_for_internal_binaries() {
    fprintf(stderr, "Test keep APPDIR environment when calling internal binaries: ");

    setup_appdir("/usr/bin");

    // grep fails if nothing matches
    assert_command_succeed(system("/usr/bin/printenv APPDIR >> /dev/null"));
    assert_command_succeed(system("/usr/bin/printenv LD_PRELOAD >> /dev/null"));

    fprintf(stderr, "Ok\n");
}

void setup_wrapper() {
    char* wrapper_path = getenv("LD_PRELOAD");

    if (wrapper_path == NULL) {
        fprintf(stderr, "Error: Missing LD_PRELOAD\n"
                        "\nUsage:\n"
                        "\tLD_PRELOAD=<libappdir-exec-wrapper.so> ./wrapper_test\n");
        exit(1);
    }

    setenv("APPIMAGE_ORIGINAL_LD_PRELOAD", "", 0);
    setenv("APPIMAGE_STARTUP_LD_PRELOAD", wrapper_path, 0);
}

int main(int argc, char** argv) {
    setup_wrapper();

    test_restore_original_env_for_external_binaries();
    test_keep_appdir_env_for_internal_binaries();

    return 0;
}

