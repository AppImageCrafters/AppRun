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

#include "file_utils.h"
#include "string_utils.h"

char** read_lines(char* filename) {
    char** result = NULL;

    FILE* fp = fopen(filename, "r");
    if (fp) {
        unsigned result_capacity = 512;
        unsigned count = 0;
        result = calloc(result_capacity, sizeof(char*));

        char* line = NULL;
        size_t len = 0;

        while (getline(&line, &len, fp) != -1) {
            if (count == result_capacity) {
                result_capacity = result_capacity * 2;
                result = extend_string_array(result, result_capacity);
            }

            result[count] = line;
            count++;
        }

        result = adjust_string_array_size(result);
        fclose(fp);
    }

    return result;
}
