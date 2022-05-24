#include <link.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <fstream>


int get_next_SO_path(dl_phdr_info* info, size_t, void* p_SO_list) {
    auto& SO_list = *static_cast<std::vector<std::string>*>(p_SO_list);
    auto p_SO_path = realpath(info->dlpi_name, nullptr);
    if (p_SO_path) {
        SO_list.emplace_back(p_SO_path);
        free(p_SO_path);
    }
    return 0;
}

std::vector<std::string> get_SO_realpaths() {
    std::vector<std::string> SO_paths;
    dl_iterate_phdr(get_next_SO_path, &SO_paths);
    return SO_paths;
}

std::string get_libc_version_from_SO_path(const std::string& SO_path) {
    std::regex rgx(R"(libc-(\d.\d+(.\d+)?))");
    std::smatch match;

    if (std::regex_search(SO_path.begin(), SO_path.end(), match, rgx))
        return match[1];

    return {};
}

std::string get_libc_version_from_file_contents(const std::string& SO_path) {
    std::regex rgx(R"(GNU C Library [^\n]* release version (\d.\d+(.\d+)?))");
    std::smatch match;
    std::string libc_version = {};

    std::ifstream so_file(SO_path);
    if (so_file.is_open()) {
        std::string line;
        while (getline(so_file, line) && libc_version.empty()) {
            if (std::regex_search(line, match, rgx))
                libc_version = match[1];
        }

        so_file.close();
    }


    return libc_version;
}

std::string get_libstdcpp_version_from_SO_path(const std::string& SO_path) {
    std::regex rgx(R"(libstdc\+\+\.so\.(\d.\d+(.\d+)?))");
    std::smatch match;

    if (std::regex_search(SO_path.begin(), SO_path.end(), match, rgx))
        return match[1];

    return {};
}

int main(int argc, char* argv[]) {
    auto SO_paths = get_SO_realpaths();
    std::string libc_version;
    std::string libstdcpp_version;

    for (auto const& SO_path : SO_paths) {
        if (libc_version.empty() && SO_path.find("libc-") != std::string::npos)
            libc_version = get_libc_version_from_SO_path(SO_path);
        if (libc_version.empty() && SO_path.find("libc.so.6") != std::string::npos)
            libc_version = get_libc_version_from_file_contents(SO_path);
        if (libstdcpp_version.empty() && SO_path.find("libstdc++.so.") != std::string::npos)
            libstdcpp_version = get_libstdcpp_version_from_SO_path(SO_path);
    }

    std::cout << "LIBC: " << libc_version << std::endl;
    std::cout << "LIBSTDC++: " << libstdcpp_version << std::endl;

    return 0;
}