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

#include "file_utils.h"
#include "string_list.h"

char **apprun_read_lines(FILE *fp) {
    int array_len = 1024;
    int count = 0;
    char **result = apprun_string_list_alloc(array_len);

    if (fp) {
        size_t len = 0;
        ssize_t read;

        while ((read = getline(&result[count], &len, fp)) != -1) {
            if (read > 0 && result[count][read - 1] == '\n')
                result[count][read - 1] = 0;
            count++;
            if (count == array_len) {
                array_len = array_len * 2;
                char **old_array = result;

                result = apprun_string_list_alloc(array_len);
                for (int i = 0; i < count; i++)
                    result[i] = old_array[i];

                free(old_array);
            }
        }
    }

    result[count] = NULL;

    return result;
}

char **apprun_file_read_lines(const char *filename) {
    char **result = NULL;

    FILE *fp = fopen(filename, "r");
    if (fp) {
        result = apprun_read_lines(fp);
        result = apprun_adjust_string_array_size(result);
        fclose(fp);
    }

    return result;
}

void apprun_file_copy(const char *source_path, const char *target_path) {
    FILE *source, *target;

    source = fopen(source_path, "r");
    target = fopen(target_path, "w");

    if (source == NULL) {
        fprintf(stderr, "Unable to read file: %s\n", source_path);
        exit(1);
    }

    if (target == NULL) {
        fprintf(stderr, "Unable to write file: %s\n", target_path);
        exit(1);
    }


    int ch;
    while ((ch = fgetc(source)) != EOF)
        fputc(ch, target);

    fclose(source);
    fclose(target);
}




