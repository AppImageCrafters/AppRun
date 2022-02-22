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
#include <sys/wait.h>
#include <linux/limits.h>

#include "../common/tests_shared.h"

int check_execv_return_code(char *path, char **argv) {
    pid_t cpid;
    int wstatus;

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        bailout();
    }

    if (cpid == 0) {            /* Code executed by child */
        int err = execv(path, argv);
        exit(err);
    } else {                    /* Code executed by parent */
        waitpid(cpid, &wstatus, 0);
        return WEXITSTATUS(wstatus);;
    }
}

void test_execv_inner_target() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *args[2] = {0x0};
    args[0] = INNER_TARGET;
    int rc = check_execv_return_code(INNER_TARGET, args);
    assert_eq(rc, 0);

    fprintf(stdout, "Ok\n");
}

void test_execv_outer_target() {
    fprintf(stdout, "%s: ", __FUNCTION__);

    char *args[2] = {0x0};
    args[0] = OUTER_TARGET;

    int rc = check_execv_return_code(OUTER_TARGET, args);
    assert_eq(rc, 0);

    fprintf(stdout, "Ok\n");
}


int main(int argc, char **argv) {
    test_execv_inner_target();
    test_execv_outer_target();

    return 0;
}

