#ifndef APPIMAGEEXECWRAPPER_RUNTIME_FILES_H
#define APPIMAGEEXECWRAPPER_RUNTIME_FILES_H

#include <stdbool.h>

int export_file_to_shm(const char* source_path, const char* target_path);

/**
 * @brief Load a given portable binary to a temporary and replace symbol values templates with the specified values, executes
 * the resulting binary checks the output and remove the temporary binary.
 *
 * @note Portable binaries are those who have been patched to allow replacing certain symbols using string replacement.
 *
 * @return 1 if the execution was successful 0 otherwise
 */
bool exec_portable_bin(char const* path, char* const argv[], char* const envp[]);

#endif //APPIMAGEEXECWRAPPER_RUNTIME_FILES_H
