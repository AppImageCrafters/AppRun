#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"

char* resolve_module_config_path() {
    char* path = malloc(FILENAME_MAX * sizeof(char));
    if (realpath("/proc/self/exe", path) == NULL) {
        fprintf(stderr, "ERROR:check-libc: Unable to find binary location");
        exit(EXIT_FAILURE);
    }

    // replace binary name by 'config'
    char* dir_slash_idx = strrchr(path, '/');
    *dir_slash_idx = 0;
    strcat(dir_slash_idx, "/config");
    if (access(path, F_OK) != 0) {
        fprintf(stderr, "ERROR:check-libc: Unable to find the config file at: %s", path);
        exit(EXIT_FAILURE);
    }

    return path;
}
