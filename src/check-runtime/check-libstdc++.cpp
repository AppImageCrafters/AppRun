#include <link.h>
#include <cstdio>
#include <cstdlib>
#include <regex.h>
#include <cstring>

static int extract_libstdcpp_version(struct dl_phdr_info* info, size_t, void* data) {
    regmatch_t match[2] = {0x0};
    auto* regex = (regex_t*) data;
    char* real_so_path = realpath(info->dlpi_name, nullptr);

    if (real_so_path != NULL && regexec(regex, real_so_path, 2, match, 0) == 0) {
        char* version = strndup(real_so_path + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
        printf("%s\n", version);
        exit(EXIT_SUCCESS);
    }
    return 0;
}

int main(int, char* []) {
    regex_t regex;
    if (regcomp(&regex, R"(libstdc\+\+\.so\.([0-9]+\.[0-9]+(\.[0-9]+)?))", REG_EXTENDED) != 0) {
        printf("Regex compilation error.");
        exit(EXIT_FAILURE);
    }

    // iterate over the loaded libraries to resolve their paths and guess the libstdc++ version from it
    dl_iterate_phdr(extract_libstdcpp_version, &regex);
    exit(EXIT_FAILURE);
}