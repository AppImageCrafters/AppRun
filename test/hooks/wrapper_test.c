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
#include <string.h>

#include "../../src/hooks/environment.h"
#include "../../src/hooks/interpreter.h"
#include "tests_shared.h"

void test_restore_original_env_for_external_binaries() {
    fprintf(stdout, "Test restore original environment when calling external binaries: ");

    set_private_env(APPRUN_ENV_APPDIR, "/tmp");

    // grep fails if nothing matches
    assert_command_fails(system("/usr/bin/printenv APPDIR >> /dev/null"));
    assert_command_fails(system("/usr/bin/printenv LD_PRELOAD >> /dev/null"));

    fprintf(stdout, "Ok\n");
}

void test_keep_appdir_env_for_internal_binaries() {
    fprintf(stdout, "Test keep APPDIR environment when calling internal binaries: ");

    set_private_env(APPRUN_ENV_APPDIR, "/usr/bin");

    // grep fails if nothing matches
    assert_command_succeed(system("/usr/bin/printenv APPDIR >> /dev/null"));
    assert_command_succeed(system("/usr/bin/printenv LD_PRELOAD >> /dev/null"));

    fprintf(stdout, "Ok\n");
}

void setup_wrapper() {
    char* wrapper_path = getenv("LD_PRELOAD");

    if (wrapper_path == NULL) {
        fprintf(stdout, "Error: Missing LD_PRELOAD\n"
                        "\nUsage:\n"
                        "\tLD_PRELOAD=<libappdir-exec-wrapper.so> ./wrapper_test\n");
        exit(1);
    }

    setenv("APPRUN_ORIGINAL_LD_PRELOAD", "", 0);
    setenv("APPRUN_STARTUP_LD_PRELOAD", wrapper_path, 0);
}

void test_prefix_interpreter_for_internal_binaries() {
    fprintf(stdout, "Test prefix INTERPRETER when calling internal binaries: ");

    set_private_env(APPRUN_ENV_APPDIR, "/bin");
    set_private_env(APPRUN_ENV_INTERPRETER, "/bin/echo");

    // grep fails if nothing matches
    assert_command_succeed(system("/bin/false >> /dev/null"));
    assert_command_succeed(system("/bin/false >> /dev/null"));

    fprintf(stdout, "Ok\n");
}

void test_dont_prefix_interpreter_for_external_binaries() {
    fprintf(stdout, "Test dont prefix INTERPRETER when calling external binaries: ");

    set_private_env(APPRUN_ENV_APPDIR, "/usr/bin");
    set_private_env(APPRUN_ENV_INTERPRETER, "/bin/echo");

    // grep fails if nothing matches
    assert_command_fails(system("/bin/false"));
    assert_command_fails(system("/bin/false"));

    fprintf(stdout, "Ok\n");
}

int main(int argc, char** argv) {
    setup_wrapper();

    test_restore_original_env_for_external_binaries();
    test_keep_appdir_env_for_internal_binaries();

    test_prefix_interpreter_for_internal_binaries();
    test_dont_prefix_interpreter_for_external_binaries();

    return 0;
}

