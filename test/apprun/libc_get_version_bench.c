/**************************************************************************
 *
 * Copyright (c) 2022 Alexis Lopez Zubieta <contact@azubieta.net>
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


#include <stdlib.h>

#include <apprun/libc_utils.h>
#include <stdio.h>
#include <time.h>

void measure_function_time(void (* _function)(), char const* _function_name) {
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    _function();

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%s took %f seconds to execute \n", _function_name, cpu_time_used);
}


void read_using_exec_call() {
    system(CHECK_RT_PATH);
}

void read_using_brute_file_parsing() {
    char* glibc_path = apprun_read_glibc_path_from_so_cache();
    char* glibc_version = apprun_read_glibc_version_from_lib(glibc_path);

    printf("glibc %s\n", glibc_version);
    free(glibc_version);
    free(glibc_path);
}


int main() {
    measure_function_time(read_using_brute_file_parsing, "read_using_brute_file_parsing");
    measure_function_time(read_using_exec_call, "read_using_exec_call");
}