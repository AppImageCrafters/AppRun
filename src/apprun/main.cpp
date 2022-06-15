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


#include <filesystem>
#include <libconfig.h++>
#include <common/appdir_environment.h>

#include "common/shell_utils.h"

#include "apprun.h"

int main(int argc, char* argv[]) {
    std::string origin_path = resolve_origin();
    apprun_set_private_env("APPDIR", origin_path, {}, origin_path);
    std::string config_path = origin_path + "/" + APPRUN_CONFIG_FILE_NAME;
    auto* settings = load_config_file(config_path);

    // load modules if required
    if (!settings->modules_dir.empty()) {
        std::string module_path = apprun_shell_expand_variables(settings->modules_dir.c_str(), argv);
        for (const auto& entry : std::filesystem::directory_iterator(module_path))
            setup_module(settings, entry);
    }

    const std::string& ld_library_path_value = generate_ld_library_path_value(settings);
    settings->environment["LD_LIBRARY_PATH"] = ld_library_path_value;

    const std::string& path_mappings_env_value = generate_path_mappings_env(settings);
    settings->environment[APPDIR_PATH_MAPPINGS_ENV] = path_mappings_env_value;

    // set runtime environment
    for (const auto& entry: settings->environment) {
        const std::string& name = entry.first;
        const std::string& value = entry.second;
        std::string orig_value = getenv(name.c_str()) != nullptr ? getenv(name.c_str()) : std::string();
        apprun_set_private_env(name, value, orig_value, value);
    }

    return launch(settings, argv);
}
