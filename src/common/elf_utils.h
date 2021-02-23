/**************************************************************************
 *
 * Copyright (c) 2021 Alexis Lopez Zubieta <contact@azubieta.net>
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

#ifndef APPIMAGEEXECWRAPPER_ELF_UTILS_H
#define APPIMAGEEXECWRAPPER_ELF_UTILS_H


#include <elf.h>
#include <stdio.h>
#include <string.h>


// Information from the ELF file relevant to AppRun
typedef struct {
    char* PT_INTER;
} APPRUN_ELF_INFO;

APPRUN_ELF_INFO* apprun_parse_elf(const char* elfFile);


#endif //APPIMAGEEXECWRAPPER_ELF_UTILS_H
