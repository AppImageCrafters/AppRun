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

#ifndef APPIMAGEEXECWRAPPER_APPRUN_H
#define APPIMAGEEXECWRAPPER_APPRUN_H

#include <vector>
#include <string>
#include <map>
#include <libconfig.h++>

#define APPRUN_CONFIG_FILE_NAME "AppRun.config"

struct AppRunSettings {
    std::string runtime_dir;
    std::string modules_dir;
    std::vector<std::string> exec;
    std::vector<std::string> library_paths;
    std::vector<std::string> path_mappings;
    std::map<std::string, std::string> environment;
};

/**
 * Resolve the directory containing the AppRun executable
 *
 * @return directory containing the AppRun executable
 */
std::string resolve_origin();

/**
 * Read config file into AppRunSettings.
 *
 * @param config_path
 * @return
 */
AppRunSettings* load_config_file(const std::string& config_path);

/**
 * Execute the module check binary and load the settings in case of failure.
 *
 * The check binary perform the sole function of verifying if the system can provide the feature (or libraries) that
 * are contained in the module.
 *
 * See the docs/MODULES.md for more details
 * @param settings
 * @param module_dir
 */
void setup_module(AppRunSettings* settings, const std::string& module_dir);


/**
 * Assemble the PATH_MAPPINGS environment variable from settings.
 *
 * Join strings from AppRunSettings.path_mappings in pair of two representing the origin and target of the mapping.
 * Two paths are join using a colon (:) to represent a mapping, different pairs are concatenated using semicolon (;).
 *
 * @param settings
 * @return PATH_MAPPINGS
 */
std::string generate_path_mappings_env(AppRunSettings* settings);

/**
 * Assemble the LD_LIBRARY_PATH environment variable from settings.
 *
 * Join strings from AppRunSettings.library_paths a colon (:) as separator.
 * See man ld.so for details about LD_LIBRARY_PATH
 *
 * @param settings
 * @return LD_LIBRARY_PATH
 */
std::string generate_ld_library_path_value(AppRunSettings* settings);

/**
 * Update environment variable. The variable will be removed if the value is empty.
 *
 * @param name
 * @param value
 */
void apprun_update_env(const std::string& name, const std::string& value);

/**
 * Set a given environment variable and the complementary original value and start value.
 *
 * See docs/PRIVATE_ENVIRONMENT.md for more details about the function of such variables.
 *
 * @param name
 * @param value
 * @param orig_value
 * @param start_value
 */
void apprun_set_private_env(const std::string& name,
                            const std::string& value,
                            const std::string& orig_value,
                            const std::string& start_value);


/**
 * Activates the private AppRun private environment and executes the command specified at AppRunSettings.exec
 *
 * The private environment setup goes as follows:
 * - Set runtime dir if required
 * - Expand variables from exec
 * - call execv
 *
 * @param settings
 * @param argv
 * @return Error in case of failure otherwise the function will not return as the process will be replaced.
 */
int launch(AppRunSettings* settings, char** argv);


#endif //APPIMAGEEXECWRAPPER_APPRUN_H
