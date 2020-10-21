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

char** apprun_read_lines(FILE* fp) {
    char** result = NULL;
    if (fp) {
        int c;

        int line_count = 1;
        while ((c = fgetc(fp)) != EOF) {
            if (c == '\n')
                line_count++;
        }

        fseek(fp, 0, SEEK_SET);
        result = apprun_string_list_alloc(line_count + 1);
        char buf[1048576];
        for (int i = 0; i < line_count; i++) {
            fgets(buf, 1048576, fp);

            // remove new line char if present
            unsigned len = strlen(buf);
            if (buf[len - 1] == '\n')
                buf[len - 1] = 0;

            result[i] = strdup(buf);
        }
    }

    return result;
}

char** apprun_file_read_lines(const char* filename) {
    char** result = NULL;

    FILE* fp = fopen(filename, "r");
    if (fp) {
        result = apprun_read_lines(fp);
        result = apprun_adjust_string_array_size(result);
        fclose(fp);
    }

    return result;
}

void apprun_file_copy(const char* source_path, const char* target_path) {
    FILE* source, * target;

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




