/**************************************************************************
 *
 * Copyright (c) 2022 Alexis Lopez Zubieta <contact@azubieta.net>
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

#include <cstdlib>
#include <cstring>
#include <climits>

#include <hooks/environment.h>
#include <common/shell_utils.h>

#include <unistd.h>
#include <iostream>


#include "apprun.h"


std::string resolve_origin() {
    std::string exec_path = realpath("/proc/self/exe", nullptr);
    std::size_t path_separator_idx = exec_path.rfind('/');
    if (path_separator_idx != std::string::npos)
        return exec_path.substr(0, path_separator_idx);

    return "";
}

int launch(AppRunSettings* settings, char** argv) {
    if (!settings->runtime_dir.empty()) {
        char* runtime_dir_value = apprun_shell_expand_variables(settings->runtime_dir.c_str(), argv);
        apprun_set_private_env(APPRUN_ENV_RUNTIME, runtime_dir_value, {}, runtime_dir_value);

        char cwd[PATH_MAX] = {0x0};
        getcwd(cwd, PATH_MAX);
        setenv(APPRUN_ENV_ORIGINAL_WORKDIR, cwd, 1);

        // set the runtime dir as working directory
        chdir(runtime_dir_value);
    }

    auto exec_argv_len = settings->exec.size();
    char** exec_argv = static_cast<char**>(malloc(sizeof(char*) * exec_argv_len + 1));
    for (int i = 0; i < exec_argv_len; i++)
        exec_argv[i] = apprun_shell_expand_variables(settings->exec[i].c_str(), argv);

    // add 0 termination
    exec_argv[exec_argv_len] = nullptr;

#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: executing ");
    for (char** itr = exec_argv; itr != nullptr && *itr != nullptr; itr++)
        fprintf(stderr, "\"%s\" ", *itr);
    fprintf(stderr, "\n");
#endif
    int ret = execv(exec_argv[0], exec_argv);
    fprintf(stderr, "APPRUN_ERROR: %s\n", strerror(errno));
    return ret;
}

std::string generate_path_mappings_env(AppRunSettings* settings) {
    std::string result;
    size_t path_mappings_len = settings->path_mappings.size();
    for (int i = 0; i < path_mappings_len / 2; i++) {
        std::string orig = apprun_shell_expand_variables(settings->path_mappings[i * 2].c_str(), nullptr);
        std::string dest = apprun_shell_expand_variables(settings->path_mappings[i * 2 + 1].c_str(), nullptr);
        result.append(orig)
                .append(":")
                .append(dest)
                .append(";");
    }

    return result;
}

std::string generate_ld_library_path_value(AppRunSettings* settings) {
    std::string result;
    for (const auto& itr : settings->library_paths) {
        std::string value = apprun_shell_expand_variables(itr.c_str(), nullptr);
        if (!result.empty())
            result += ":";

        result += value;
    }

    const char* ld_library_path_original_value = getenv("LD_LIBRARY_PATH");
    if (ld_library_path_original_value != nullptr) {
        if (!result.empty())
            result += ":";
        result += ld_library_path_original_value;

    }

    return result;
}

/**
 * Read library paths from module config file into AppRunSettings
 * @param settings
 * @param module_config
 */
void import_module_library_paths(AppRunSettings* settings, const libconfig::Config& module_config) {
    auto& module_library_paths_setting = module_config.lookup("module.library_paths");
    auto module_library_paths_setting_len = module_library_paths_setting.getLength();
    for (int i = 0; i < module_library_paths_setting_len; i++) {
        std::string library_path = module_library_paths_setting[i];
        settings->library_paths.push_back(library_path);
    }
}

/**
 * Read environment from module config file into AppRunSettings
 * @param settings
 * @param module_config
 */
void import_module_environment(AppRunSettings* const settings, const libconfig::Config& module_config) {
    auto& module_environment_setting = module_config.lookup("module.environment");
    auto module_environment_setting_len = module_environment_setting.getLength();
    for (int i = 0; i < module_environment_setting_len; i++) {
        std::string name = module_environment_setting[i].getName();
        std::string value = module_environment_setting[i];

        if (settings->environment.find(name) != settings->environment.end())
            std::cerr << "APPRUN WARNING: Overriding Environment " << name << std::endl;

        settings->environment[name] = value;
    }
}

void setup_module(AppRunSettings* settings, const std::string& module_dir) {
    // verify if the system has the required feature
    auto module_check_path = module_dir + "/check";

    // ignore folders without check binary
    if (access(module_check_path.c_str(), F_OK) != 0)
        return;

    std::cout << "Checking module " << module_dir << std::endl;
    if (system(module_check_path.c_str()) != EXIT_SUCCESS) {
        std::cout << "Enabling module " << module_dir << std::endl;
        // enable module
        auto module_config_path = module_dir + "/config";

        libconfig::Config module_config;
        try {
            module_config.readFile(module_config_path);
            if (module_config.exists("module.library_paths"))
                import_module_library_paths(settings, module_config);

            if (module_config.exists("module.environment"))
                import_module_environment(settings, module_config);

            if (module_config.exists("module.runtime_dir")) {
                std::string value = module_config.lookup("module.runtime_dir");
                settings->runtime_dir = value;
            }

        } catch (libconfig::ParseException& ex) {
            std::cerr << ex.getFile() << ":" << ex.getLine() << ":" << ex.getError() << "\n";
            exit(EXIT_FAILURE);
        } catch (libconfig::SettingTypeException& ex) {
            std::cerr << ex.getPath() << "\n";
            exit(EXIT_FAILURE);
        }
    }
}


AppRunSettings* load_config_file(const std::string& config_path) {
    // ignore folders without check binary
    if (access(config_path.c_str(), F_OK) != 0) {
        std::cerr << "Missing config file: " << config_path << "\n";
        exit(EXIT_FAILURE);
    }

    auto* settings = new AppRunSettings();

    // load config file
    libconfig::Config apprun_config;
    apprun_config.readFile(config_path);
    auto& runtime_settings = apprun_config.lookup("runtime");

    // load exec line (required)
    auto& exec_line = runtime_settings.lookup("exec");
    size_t exec_line_len = exec_line.getLength();
    for (int i = 0; i < exec_line_len; i++) {
        std::string entry = exec_line[i];
        settings->exec.push_back(entry);
    }

    // load modules_dir (optional)
    if (runtime_settings.exists("modules_dir")) {
        std::string modules_dir = runtime_settings.lookup("modules_dir");
        settings->modules_dir = modules_dir;
    }

    // load library_paths (optional)
    if (runtime_settings.exists("library_paths")) {
        auto& library_paths = runtime_settings.lookup("library_paths");
        size_t library_paths_len = library_paths.getLength();
        for (int i = 0; i < library_paths_len; i++) {
            std::string value = library_paths[i];
            settings->library_paths.push_back(value);
        }
    }

    // load path_mappings (optional)
    if (runtime_settings.exists("path_mappings")) {
        auto& path_mappings = runtime_settings.lookup("path_mappings");
        size_t path_mappings_len = path_mappings.getLength();
        for (int i = 0; i < path_mappings_len; i++) {
            std::string value = path_mappings[i];
            settings->path_mappings.push_back(value);
        }
    }

    // load environment (optional)
    if (runtime_settings.exists("environment")) {
        auto& environment = runtime_settings.lookup("environment");
        size_t environment_len = environment.getLength();
        for (int i = 0; i < environment_len; i++) {
            std::string key = environment[i].getName();
            std::string value = environment[i];

            settings->environment[key] = value;
        }
    }

    // set default runtime dir
    settings->runtime_dir = strdup(getenv("APPDIR"));

    return settings;
}

void apprun_update_env(const std::string& name, const std::string& value) {
    if (!value.empty())
        setenv(name.c_str(), value.c_str(), 1);
    else
        unsetenv(name.c_str());
}

void apprun_set_private_env(const std::string& name,
                            const std::string& value,
                            const std::string& orig_value,
                            const std::string& start_value) {
    apprun_update_env(name, value);

    std::string orig_name = APPRUN_ENV_ORIG_PREFIX + name;
    apprun_update_env(orig_name, orig_value);


    std::string startup_name = APPRUN_ENV_STARTUP_PREFIX + name;
    apprun_update_env(startup_name, start_value);

#ifdef DEBUG
    fprintf(stderr, "APPRUN_DEBUG: set env %s=%s\n", name.c_str(), value.c_str());
#endif
}
