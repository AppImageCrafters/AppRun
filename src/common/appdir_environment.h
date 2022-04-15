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

#ifndef APPIMAGEEXECWRAPPER_APPDIR_ENVIRONMENT_H
#define APPIMAGEEXECWRAPPER_APPDIR_ENVIRONMENT_H

#define APPDIR_PATH_MAPPINGS_ENV "APPDIR_PATH_MAPPINGS"

#define APPDIR_EXEC_PATH_ENV "APPDIR_EXEC_PATH"
#define APPDIR_EXEC_ARGS_ENV "APPDIR_EXEC_ARGS"
#define APPDIR_LIBRARY_PATH_ENV "APPDIR_LIBRARY_PATH"
#define APPDIR_LIBC_PREFIX_ENV "APPDIR_LIBC_PREFIX"
#define APPDIR_LIBC_VERSION_ENV "APPDIR_LIBC_VERSION"
#define APPDIR_LIBC_LINKER_PATH_ENV "APPDIR_LIBC_LINKER_PATH"
#define APPDIR_LIBC_LIBRARY_PATH_ENV "APPDIR_LIBC_LIBRARY_PATH"
#define APPDIR_LIBC_LINKER_PATH_ENV_SEPARATOR ":"

#define APPDIR_MODULE_DIR_ENV "APPDIR_MODULE_DIR"
#define APPDIR_MODULE_DIR_ENV_DELIM ":"

#define APPDIR_FORCE_BUNDLE_RUNTIME_ENV "APPDIR_FORCE_BUNDLE_RUNTIME"

#endif //APPIMAGEEXECWRAPPER_APPDIR_ENVIRONMENT_H