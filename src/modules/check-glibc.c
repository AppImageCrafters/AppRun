#include <stdio.h>
#include <libconfig.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gnu/libc-version.h>

#include "common/string_utils.h"

char* resolve_config_path() {
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

char* read_required_glibc_version_from_config(char* config_path) {
    config_t cfg;
    const char* str;

    config_init(&cfg);

    /* Read the file. If there is an error, report it and exit. */
    if (!config_read_file(&cfg, config_path)) {
        fprintf(stderr, "ERROR:check-libc: %s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg),
                config_error_text(&cfg));
        config_destroy(&cfg);
        exit(EXIT_FAILURE);
    }

    if (!config_lookup_string(&cfg, "check.required_glibc", &str)) {
        fprintf(stderr, "ERROR:check-libc: missing config entry 'check.required_glibc'");
        exit(EXIT_FAILURE);
    }

    char* glibc_version = strdup(str);

    config_destroy(&cfg);
    return glibc_version;
}

int main() {
    char* config_path = resolve_config_path();
    const char* required_glibc_version = read_required_glibc_version_from_config(config_path);
    const char* system_glibc_version = gnu_get_libc_version();

    // the libc module must be used if the system version is lesser than the required
    if (compare_version_strings(system_glibc_version, required_glibc_version) > 0)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}