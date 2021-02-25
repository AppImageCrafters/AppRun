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
#include "string_utils.h"

#include <stdlib.h>
#include <assert.h>
#include <archive.h>

char* apprun_elf_32_load_str_table(FILE* file, const Elf32_Shdr* str_table_shdr);

char* apprun_elf_64_load_str_table(FILE* file, const Elf64_Shdr* str_table_shdr);

unsigned char* apprun_elf_require_ident(FILE* f) {
    unsigned char* e_ident = malloc(EI_NIDENT);
    const unsigned char expected_e_ident[] = {0x7F, 'E', 'L', 'F'};

    fseek(f, 0, SEEK_SET);
    fread(e_ident, sizeof(unsigned char), EI_NIDENT, f);

    if (memcmp(e_ident, expected_e_ident, 4) != 0) {
        free(e_ident);
        return NULL;
    } else
        return e_ident;
}

// Read PT_INTERP implementations

char* apprun_elf_32_read_pt_interp(FILE* file, const Elf32_Ehdr* ehdr);

char* apprun_elf_64_read_pt_interp(FILE* file, const Elf64_Ehdr* ehdr);

char* apprun_elf_read_pt_interp(const char* path) {
    FILE* elf = fopen(path, "rb");
    unsigned char* e_ident = apprun_elf_require_ident(elf);
    if (e_ident == NULL) {
        fprintf(stderr, "APPRUN_ERROR: Missing ELF ident on: %s", path);
        exit(1);
    }

    // select implementation according to the class
    if (e_ident[EI_CLASS] == ELFCLASS32) {
        Elf32_Ehdr ehdr;
        fseek(elf, 0, SEEK_SET);
        fread(&ehdr, sizeof(Elf32_Ehdr), 1, elf);

        return apprun_elf_32_read_pt_interp(elf, &ehdr);
    }

    if (e_ident[EI_CLASS] == ELFCLASS64) {
        Elf64_Ehdr ehdr;
        fseek(elf, 0, SEEK_SET);
        fread(&ehdr, sizeof(Elf64_Ehdr), 1, elf);

        return apprun_elf_64_read_pt_interp(elf, &ehdr);
    }

    fprintf(stderr, "APPRUN_ERROR: Unknown ELF class '%d' on: %s", e_ident[EI_CLASS], path);
    exit(1);
}

char* apprun_elf_32_read_pt_interp(FILE* file, const Elf32_Ehdr* ehdr) {
    // Iterate program header entries
    Elf32_Off phdr_ent_itr = ehdr->e_phoff;
    Elf32_Off phdr_ent_itr_end = ehdr->e_phoff + (ehdr->e_phentsize * ehdr->e_phnum);

    for (; phdr_ent_itr < phdr_ent_itr_end; phdr_ent_itr += ehdr->e_phentsize) {
        fseek(file, phdr_ent_itr, SEEK_SET);
        Elf32_Phdr phdr_ent;
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

char* apprun_elf_64_read_pt_interp(FILE* file, const Elf64_Ehdr* ehdr) {
    // Iterate program header entries
    Elf64_Off phdr_ent_itr = ehdr->e_phoff;
    Elf64_Off phdr_ent_itr_end = ehdr->e_phoff + (ehdr->e_phentsize * ehdr->e_phnum);

    for (; phdr_ent_itr < phdr_ent_itr_end; phdr_ent_itr += ehdr->e_phentsize) {
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


// Read GLIBC version implementations

char* apprun_elf_32_read_glibc_version(FILE* file, Elf32_Ehdr* ehdr);

char* apprun_elf_64_read_glibc_version(FILE* file, Elf64_Ehdr* hdr);

char* apprun_elf_read_glibc_version(const char* path) {
    FILE* elf = fopen(path, "rb");
    unsigned char* e_ident = apprun_elf_require_ident(elf);
    if (e_ident == NULL) {
        fprintf(stderr, "APPRUN_ERROR: Missing ELF ident on: %s", path);
        exit(1);
    }

    // select implementation according to the class
    if (e_ident[EI_CLASS] == ELFCLASS32) {
        Elf32_Ehdr ehdr;
        fseek(elf, 0, SEEK_SET);
        fread(&ehdr, sizeof(Elf32_Ehdr), 1, elf);

        return apprun_elf_32_read_glibc_version(elf, &ehdr);
    }

    if (e_ident[EI_CLASS] == ELFCLASS64) {
        Elf64_Ehdr ehdr;
        fseek(elf, 0, SEEK_SET);
        fread(&ehdr, sizeof(Elf64_Ehdr), 1, elf);

        return apprun_elf_64_read_glibc_version(elf, &ehdr);
    }

    fprintf(stderr, "APPRUN_ERROR: Unknown ELF class '%d' on: %s", e_ident[EI_CLASS], path);
    exit(1);
}

char* apprun_elf_32_read_glibc_version(FILE* file, Elf32_Ehdr* ehdr) {
    char glibc_version[254] = {0x0};

    // load section headers table
    Elf32_Shdr shdr_table[ehdr->e_shnum];
    fseek(file, ehdr->e_shoff, SEEK_SET);
    fread(&shdr_table, sizeof(Elf32_Shdr), ehdr->e_shnum, file);

    // Iterate section header entries
    for (int idx = 0; idx < ehdr->e_shnum; idx++) {
        const Elf32_Shdr* shdr_ent = &shdr_table[idx];
        if (shdr_ent->sh_type == SHT_DYNSYM) {
            // load linked strings table
            const Elf32_Shdr* linked_str_table_shdr = &shdr_table[shdr_ent->sh_link];
            char* linked_str_table = apprun_elf_32_load_str_table(file, linked_str_table_shdr);

            Elf32_Xword entries_count = shdr_ent->sh_size / sizeof(Elf32_Sym);
            Elf32_Sym entries[entries_count];

            fseek(file, shdr_ent->sh_offset, SEEK_SET);
            fread(entries, sizeof(Elf32_Sym), entries_count, file);

            // Find global object symbols with GLIBC_ prefix assuming they will contain a version string after
            for (int i = 0; i < entries_count; i++) {
                const char* symbol_name = linked_str_table + entries[i].st_name;
                if (ELF64_ST_BIND(entries[i].st_info) == STB_GLOBAL &&
                    ELF64_ST_TYPE(entries[i].st_info) == STT_OBJECT &&
                    strncmp("GLIBC_", symbol_name, 6) == 0) {

                    // chop "GLIBC_" prefix
                    const char* version_str = symbol_name + 6;
                    if (version_str != NULL && apprun_compare_version_strings(version_str, glibc_version) > 0) {
                        strcpy(glibc_version, version_str);
                    }
                }
            }
            free(linked_str_table);
        }
    }

    return strdup(glibc_version);
}

char* apprun_elf_64_read_glibc_version(FILE* file, Elf64_Ehdr* hdr) {
    char glibc_version[254] = {0x0};

    // load section headers table
    Elf64_Shdr shdr_table[hdr->e_shnum];
    fseek(file, hdr->e_shoff, SEEK_SET);
    fread(&shdr_table, sizeof(Elf64_Shdr), hdr->e_shnum, file);

    // Iterate section header entries
    for (int idx = 0; idx < hdr->e_shnum; idx++) {
        const Elf64_Shdr* shdr_ent = &shdr_table[idx];
        if (shdr_ent->sh_type == SHT_DYNSYM) {
            // load linked strings table
            const Elf64_Shdr* linked_str_table_shdr = &shdr_table[shdr_ent->sh_link];
            char* linked_str_table = apprun_elf_64_load_str_table(file, linked_str_table_shdr);

            Elf64_Xword entries_count = shdr_ent->sh_size / sizeof(Elf64_Sym);
            Elf64_Sym entries[entries_count];

            fseek(file, shdr_ent->sh_offset, SEEK_SET);
            fread(entries, sizeof(Elf64_Sym), entries_count, file);

            // Find global object symbols with GLIBC_ prefix assuming they will contain a version string after
            for (int i = 0; i < entries_count; i++) {
                const char* symbol_name = linked_str_table + entries[i].st_name;
                if (ELF64_ST_BIND(entries[i].st_info) == STB_GLOBAL &&
                    ELF64_ST_TYPE(entries[i].st_info) == STT_OBJECT &&
                    strncmp("GLIBC_", symbol_name, 6) == 0) {

                    // chop "GLIBC_" prefix
                    const char* version_str = symbol_name + 6;
                    if (version_str != NULL && apprun_compare_version_strings(version_str, glibc_version) > 0) {
                        strcpy(glibc_version, version_str);
                    }
                }
            }
            free(linked_str_table);
        }
    }

    return strdup(glibc_version);
}

char* apprun_elf_64_load_str_table(FILE* file, const Elf64_Shdr* str_table_shdr) {
    char* str_table = malloc(str_table_shdr->sh_size);
    fseek(file, (*str_table_shdr).sh_offset, SEEK_SET);
    fread(str_table, 1, (*str_table_shdr).sh_size, file);

    return str_table;
}

char* apprun_elf_32_load_str_table(FILE* file, const Elf32_Shdr* str_table_shdr) {
    char* str_table = malloc(str_table_shdr->sh_size);
    fseek(file, (*str_table_shdr).sh_offset, SEEK_SET);
    fread(str_table, 1, (*str_table_shdr).sh_size, file);

    return str_table;
}
