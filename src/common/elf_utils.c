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

#include "elf_utils.h"

#include <stdlib.h>

char* apprun_read_elf_64_pt_interp(FILE* file, Elf64_Ehdr h);

APPRUN_ELF_INFO* apprun_parse_elf(const char* elfFile) {
    APPRUN_ELF_INFO* info = NULL;

    // Either Elf64_Ehdr or Elf32_Ehdr depending on architecture.
    Elf64_Ehdr header;

    FILE* file = fopen(elfFile, "rb");
    if (file) {
        // read the header
        fread(&header, sizeof(header), 1, file);

        // check so its really an elf file
        if (memcmp(header.e_ident, ELFMAG, SELFMAG) == 0) {
            info = malloc(sizeof(APPRUN_ELF_INFO));
            info->interpreter_path = apprun_read_elf_64_pt_interp(file, header);
        }

        // finally close the file
        fclose(file);
    }

    return info;
}

char* apprun_read_elf_64_pt_interp(FILE* file, Elf64_Ehdr h) {
    // Iterate program header entries
    Elf64_Off phdr_ent_itr = h.e_phoff;
    Elf64_Off phdr_ent_itr_end = h.e_phoff + (h.e_phentsize * h.e_phnum);

    for (; phdr_ent_itr < phdr_ent_itr_end; phdr_ent_itr += h.e_phentsize) {
        fseek(file, phdr_ent_itr, SEEK_SET);
        Elf64_Phdr phdr_ent;
        fread(&phdr_ent, sizeof(phdr_ent), 1, file);
        if (phdr_ent.p_type == PT_INTERP) {
            fseek(file, phdr_ent.p_offset, SEEK_SET);

            char* data = malloc(phdr_ent.p_filesz + 1);
            memset(data, 0, phdr_ent.p_filesz + 1);
            fread(data, phdr_ent.p_filesz, 1, file);
#ifdef DEBUG
            fprintf(stderr, "APPRUN_DEBUG: ELF PT_INTERP \"%s\"\n", data);
#endif
            return data;
        }
    }
    return NULL;
}
