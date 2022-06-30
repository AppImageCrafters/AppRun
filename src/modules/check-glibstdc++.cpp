#include <link.h>
#include <cstdlib>
#include <regex.h>
#include <cstring>
#include <libconfig.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"
#include "common/string_utils.h"

#ifdef __cplusplus
}
#endif

typedef struct {
    regex_t regex;
    char* version;
} extract_libstdcpp_version_ctx_t;

static int extract_libstdcpp_version(struct dl_phdr_info* info, size_t, void* data) {
    regmatch_t match[2] = {0x0};
    auto* ctx = (extract_libstdcpp_version_ctx_t*) data;
    char* real_so_path = realpath(info->dlpi_name, nullptr);

    if (real_so_path != NULL && regexec(&ctx->regex, real_so_path, 2, match, 0) == 0) {
        char* version = strndup(real_so_path + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
        ctx->version = version;
        return 1;
    }

    return 0;
}

char* extract_libstdcpp_version_from_runtime_libaries_paths() {
    extract_libstdcpp_version_ctx_t extract_libstdcpp_version_ctx = {nullptr};

    if (regcomp(&extract_libstdcpp_version_ctx.regex, R"(libstdc\+\+\.so\.([0-9]+\.[0-9]+(\.[0-9]+)?))",
                REG_EXTENDED) != 0) {
        printf("Regex compilation error.");
        exit(EXIT_FAILURE);
    }

    // iterate over the loaded libraries to resolve their paths and guess the libstdc++ version from it
    dl_iterate_phdr(extract_libstdcpp_version, &extract_libstdcpp_version_ctx);
    return extract_libstdcpp_version_ctx.version;
}

char* read_required_glibstdcpp_version_from_config(char* config_path) {
    config_t cfg;
    const char* str;

    config_init(&cfg);

    /* Read the file. If there is an error, report it and exit. */
    if (!config_read_file(&cfg, config_path)) {
        fprintf(stderr, "ERROR:check-glibstdc++: %s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg),
                config_error_text(&cfg));
        config_destroy(&cfg);
        exit(EXIT_FAILURE);
    }

    if (!config_lookup_string(&cfg, "check.required_glibstdcpp", &str)) {
        fprintf(stderr, "ERROR:check-glibstdc++: missing config entry 'check.required_glibstdcpp'");
        exit(EXIT_FAILURE);
    }

    char* glibstdcpp_version = strdup(str);

    config_destroy(&cfg);
    return glibstdcpp_version;
}

int main(int, char* []) {
    char* system_version = extract_libstdcpp_version_from_runtime_libaries_paths();

    char* config_path = resolve_module_config_path();
    char* required_version = read_required_glibstdcpp_version_from_config(config_path);

    // the libc module must be used if the system version is lesser than the required
    if (compare_version_strings(system_version, required_version) > 0)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}