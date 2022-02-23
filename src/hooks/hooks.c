/**************************************************************************
 *
 * Copyright (C) 2016 Sven Brauch <mail@svenbrauch.de>
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

#define _GNU_SOURCE
#define __USE_GNU

#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/socket.h>
#include <sys/statvfs.h>
#include <sys/un.h>
#include <sys/vfs.h>
#include <unistd.h>

#include "redirect_path.h"

#define REDIRECT_1_1(RET, NAME) \
RET \
NAME (const char *path) \
{ \
    RET (*_NAME) (const char *path); \
    char *new_path = NULL; \
    RET result; \
    _NAME = (RET (*)(const char *path)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path (path); \
    result = _NAME (new_path); \
    free (new_path); \
    return result; \
}

#define REDIRECT_1_2(RET, NAME, T2) \
RET \
NAME (const char *path, T2 A2) \
{ \
    RET (*_NAME) (const char *path, T2 A2); \
    char *new_path = NULL; \
    RET result; \
    _NAME = (RET (*)(const char *path, T2 A2)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path (path); \
    result = _NAME (new_path, A2); \
    free (new_path); \
    return result; \
}

#define REDIRECT_1_3(RET, NAME, T2, T3) \
RET \
NAME (const char *path, T2 A2, T3 A3) \
{ \
    RET (*_NAME) (const char *path, T2 A2, T3 A3); \
    char *new_path = NULL; \
    RET result; \
    _NAME = (RET (*)(const char *path, T2 A2, T3 A3)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path (path); \
    result = _NAME (new_path, A2, A3); \
    free (new_path); \
    return result; \
}

#define REDIRECT_2_2(RET, NAME, T1) \
RET \
NAME (T1 A1, const char *path) \
{ \
    RET (*_NAME) (T1 A1, const char *path); \
    char *new_path = NULL; \
    RET result; \
    _NAME = (RET (*)(T1 A1, const char *path)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path (path); \
    result = _NAME (A1, new_path); \
    free (new_path); \
    return result; \
}

#define REDIRECT_2_3(RET, NAME, T1, T3) \
RET \
NAME (T1 A1, const char *path, T3 A3) \
{ \
    RET (*_NAME) (T1 A1, const char *path, T3 A3); \
    char *new_path = NULL; \
    RET result; \
    _NAME = (RET (*)(T1 A1, const char *path, T3 A3)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path (path); \
    result = _NAME (A1, new_path, A3); \
    free (new_path); \
    return result; \
}

#define REDIRECT_2_3_AT(RET, NAME, T1, T3) \
RET \
NAME (T1 A1, const char *path, T3 A3) \
{ \
    RET (*_NAME) (T1 A1, const char *path, T3 A3); \
    char *new_path = NULL; \
    RET result; \
    _NAME = (RET (*)(T1 A1, const char *path, T3 A3)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path_if_absolute (path); \
    result = _NAME (A1, new_path, A3); \
    free (new_path); \
    return result; \
}

#define REDIRECT_2_4_AT(RET, NAME, T1, T3, T4) \
RET \
NAME (T1 A1, const char *path, T3 A3, T4 A4) \
{ \
    RET (*_NAME) (T1 A1, const char *path, T3 A3, T4 A4); \
    char *new_path = NULL; \
    RET result; \
    _NAME = (RET (*)(T1 A1, const char *path, T3 A3, T4 A4)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path_if_absolute (path); \
    result = _NAME (A1, new_path, A3, A4); \
    free (new_path); \
    return result; \
}

#define REDIRECT_3_5(RET, NAME, T1, T2, T4, T5) \
RET \
NAME (T1 A1, T2 A2, const char *path, T4 A4, T5 A5) \
{ \
    RET (*_NAME) (T1 A1, T2 A2, const char *path, T4 A4, T5 A5); \
    char *new_path = NULL; \
    RET result; \
    _NAME = (RET (*)(T1 A1, T2 A2, const char *path, T4 A4, T5 A5)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path (path); \
    result = _NAME (A1, A2, new_path, A4, A5); \
    free (new_path); \
    return result; \
}

#define REDIRECT_TARGET(RET, NAME) \
RET \
NAME (const char *path, const char *target) \
{ \
    RET (*_NAME) (const char *path, const char *target); \
    char *new_path = NULL; \
    char *new_target = NULL; \
    RET result; \
    _NAME = (RET (*)(const char *path, const char *target)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path (path); \
    new_target = apprun_redirect_path_target (target); \
    result = _NAME (new_path, new_target); \
    free (new_path); \
    free (new_target); \
    return result; \
}

#define REDIRECT_OPEN(NAME) \
int \
NAME (const char *path, int flags, ...) \
{ \
    int mode = 0; \
    int (*_NAME) (const char *path, int flags, mode_t mode); \
    char *new_path = NULL; \
    int result; \
    if (flags & (O_CREAT|O_TMPFILE)) \
    { \
        va_list ap; \
        va_start (ap, flags); \
        mode = va_arg (ap, mode_t); \
        va_end (ap); \
    } \
    _NAME = (int (*)(const char *path, int flags, mode_t mode)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path (path); \
    result = _NAME (new_path, flags, mode); \
    free (new_path); \
    return result; \
}

#define REDIRECT_OPEN_AT(NAME) \
int \
NAME (int dirfp, const char *path, int flags, ...) \
{ \
    int mode = 0; \
    int (*_NAME) (int dirfp, const char *path, int flags, mode_t mode); \
    char *new_path = NULL; \
    int result; \
    if (flags & (O_CREAT|O_TMPFILE)) \
    { \
        va_list ap; \
        va_start (ap, flags); \
        mode = va_arg (ap, mode_t); \
        va_end (ap); \
    } \
    _NAME = (int (*)(int dirfp, const char *path, int flags, mode_t mode)) dlsym (RTLD_NEXT, #NAME); \
    new_path = apprun_redirect_path_if_absolute (path); \
    result = _NAME (dirfp, new_path, flags, mode); \
    free (new_path); \
    return result; \
}

REDIRECT_1_2(FILE *, fopen, const char *)

REDIRECT_1_2(FILE *, fopen64, const char *)

REDIRECT_1_1(int, unlink)

REDIRECT_2_3_AT(int, unlinkat, int, int)

REDIRECT_1_2(int, access, int)

REDIRECT_1_2(int, eaccess, int)

REDIRECT_1_2(int, euidaccess, int)

REDIRECT_2_4_AT(int, faccessat, int, int, int)

REDIRECT_1_2(int, stat, struct stat *)

REDIRECT_1_2(int, stat64, struct stat64 *)

REDIRECT_1_2(int, lstat, struct stat *)

REDIRECT_1_2(int, lstat64, struct stat64 *)

REDIRECT_1_2(int, creat, mode_t)

REDIRECT_1_2(int, creat64, mode_t)

REDIRECT_1_2(int, truncate, off_t)

REDIRECT_2_2(char *, bindtextdomain, const char *)

REDIRECT_2_3(int, __xstat, int, struct stat *)

REDIRECT_2_3(int, __xstat64, int, struct stat64 *)

REDIRECT_2_3(int, __lxstat, int, struct stat *)

REDIRECT_2_3(int, __lxstat64, int, struct stat64 *)

REDIRECT_3_5(int, __fxstatat, int, int, struct stat *, int)

REDIRECT_3_5(int, __fxstatat64, int, int, struct stat64 *, int)

REDIRECT_1_2(int, statfs, struct statfs *)

REDIRECT_1_2(int, statfs64, struct statfs64 *)

REDIRECT_1_2(int, statvfs, struct statvfs *)

REDIRECT_1_2(int, statvfs64, struct statvfs64 *)

REDIRECT_1_2(long, pathconf, int)

REDIRECT_1_1(DIR *, opendir)

REDIRECT_1_2(int, mkdir, mode_t)

REDIRECT_1_1(int, rmdir)

REDIRECT_1_3(int, chown, uid_t, gid_t)

REDIRECT_1_3(int, lchown, uid_t, gid_t)

REDIRECT_1_2(int, chmod, mode_t)

REDIRECT_1_2(int, lchmod, mode_t)

REDIRECT_1_1(int, chdir)

REDIRECT_1_3(ssize_t, readlink, char *, size_t)

REDIRECT_TARGET(int, link)

REDIRECT_TARGET(int, rename)

REDIRECT_OPEN(open)

REDIRECT_OPEN(open64)

REDIRECT_OPEN_AT(openat)

REDIRECT_OPEN_AT(openat64)

REDIRECT_2_3(int, inotify_add_watch, int, uint32_t)

int
scandir(const char *dirp, struct dirent ***namelist, int (*filter)(const struct dirent *),
        int (*compar)(const struct dirent **, const struct dirent **)) {
    int (*_scandir)(const char *dirp, struct dirent ***namelist, int (*filter)(const struct dirent *),
                    int (*compar)(const struct dirent **, const struct dirent **));
    char *new_path = NULL;
    int ret;

    _scandir = (int (*)(const char *dirp, struct dirent ***namelist, int (*filter)(const struct dirent *),
                        int (*compar)(const struct dirent **, const struct dirent **))) dlsym(RTLD_NEXT, "scandir");

    new_path = apprun_redirect_path(dirp);
    ret = _scandir(new_path, namelist, filter, compar);
    free(new_path);

    return ret;
}

int
scandir64(const char *dirp, struct dirent64 ***namelist,
          int (*filter)(const struct dirent64 *),
          int (*compar)(const struct dirent64 **, const struct dirent64 **)) {
    int (*_scandir64)(const char *dirp, struct dirent64 ***namelist,
                      int (*filter)(const struct dirent64 *),
                      int (*compar)(const struct dirent64 **, const struct dirent64 **));
    char *new_path = NULL;
    int ret;

    _scandir64 = (int (*)(const char *dirp, struct dirent64 ***namelist,
                          int (*filter)(const struct dirent64 *),
                          int (*compar)(const struct dirent64 **, const struct dirent64 **)))
            dlsym(RTLD_NEXT, "scandir64");

    new_path = apprun_redirect_path(dirp);
    ret = _scandir64(new_path, namelist, filter, compar);
    free(new_path);

    return ret;
}


int
scandirat(int dirfd, const char *dirp, struct dirent ***namelist, int (*filter)(const struct dirent *),
          int (*compar)(const struct dirent **, const struct dirent **)) {
    int (*_scandirat)(int dirfd, const char *dirp, struct dirent ***namelist, int (*filter)(const struct dirent *),
                      int (*compar)(const struct dirent **, const struct dirent **));
    char *new_path = NULL;
    int ret;

    _scandirat = (int (*)(int dirfd, const char *dirp, struct dirent ***namelist, int (*filter)(const struct dirent *),
                          int (*compar)(const struct dirent **, const struct dirent **))) dlsym(RTLD_NEXT, "scandirat");

    new_path = apprun_redirect_path_if_absolute(dirp);
    ret = _scandirat(dirfd, new_path, namelist, filter, compar);
    free(new_path);

    return ret;
}

int
scandirat64(int dirfd, const char *dirp, struct dirent64 ***namelist,
            int (*filter)(const struct dirent64 *),
            int (*compar)(const struct dirent64 **, const struct dirent64 **)) {
    int (*_scandirat64)(int dirfd, const char *dirp, struct dirent64 ***namelist,
                        int (*filter)(const struct dirent64 *),
                        int (*compar)(const struct dirent64 **, const struct dirent64 **));
    char *new_path = NULL;
    int ret;

    _scandirat64 = (int (*)(int dirfd, const char *dirp, struct dirent64 ***namelist,
                            int (*filter)(const struct dirent64 *),
                            int (*compar)(const struct dirent64 **, const struct dirent64 **)))
            dlsym(RTLD_NEXT, "scandirat64");

    new_path = apprun_redirect_path_if_absolute(dirp);
    ret = _scandirat64(dirfd, new_path, namelist, filter, compar);
    free(new_path);

    return ret;
}

int
bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int (*_bind)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    int result;

    _bind = (int (*)(int sockfd, const struct sockaddr *addr, socklen_t addrlen)) dlsym(RTLD_NEXT, "bind");

    if (addr->sa_family == AF_UNIX &&
        ((const struct sockaddr_un *) addr)->sun_path[0] != 0) { // could be abstract socket
        char *new_path = NULL;
        struct sockaddr_un new_addr;

        new_path = apprun_redirect_path(((const struct sockaddr_un *) addr)->sun_path);

        new_addr.sun_family = AF_UNIX;
        strcpy(new_addr.sun_path, new_path);
        free(new_path);

        result = _bind(sockfd, (const struct sockaddr *) &new_addr, sizeof(new_addr));
    } else {
        result = _bind(sockfd, addr, addrlen);
    }

    return result;
}

int
connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int (*_connect)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

    _connect = (int (*)(int sockfd, const struct sockaddr *addr, socklen_t addrlen)) dlsym(RTLD_NEXT, "connect");

    /* addrlen == sizeof(sa_family_t) is the case of unnamed sockets,
     * and first byte of sun_path is 0 for abstract sockets.
     */
    if (addr->sa_family == AF_UNIX
        && addrlen > sizeof(sa_family_t)
        && ((const struct sockaddr_un *) addr)->sun_path[0] != '\0') {

        const struct sockaddr_un *un_addr = (const struct sockaddr_un *) addr;
        char *new_path = NULL;
        struct sockaddr_un new_addr;

        new_path = apprun_redirect_path(un_addr->sun_path);

        new_addr.sun_family = AF_UNIX;
        strcpy(new_addr.sun_path, new_path);
        free(new_path);

        return _connect(sockfd, (const struct sockaddr *) &new_addr, sizeof(new_addr));
    }

    return _connect(sockfd, addr, addrlen);
}

void *
dlopen(const char *path, int mode) {
    void *(*_dlopen)(const char *path, int mode);
    char *new_path = NULL;
    void *result;

    _dlopen = (void *(*)(const char *path, int mode)) dlsym(RTLD_NEXT, "dlopen");

    if (path && path[0] == '/') {
        new_path = apprun_redirect_path(path);
        result = _dlopen(new_path, mode);
        free(new_path);
    } else {
        // non-absolute library paths aren't simply relative paths, they need
        // a whole lookup algorithm
        result = _dlopen(path, mode);
    }

    return result;
}

char *
realpath(const char *path, char *resolved_path) {
    char *(*_realpath)(const char *, char *);
    char *new_path = NULL;
    char *result;
    _realpath = (char *(*)(const char *, char *)) dlsym(RTLD_NEXT, "realpath");
    new_path = apprun_redirect_path(path);

    // The  resolved_path == NULL feature, not standardized in POSIX.1-2001, but standardized in POSIX.1-2008.
    // Try to follow the POSIX.1-2008 standard
    char *_resolved_path = resolved_path;
    if (_resolved_path == NULL)
        _resolved_path = malloc(PATH_MAX);

    result = _realpath(new_path, _resolved_path);
    free(new_path);

    if (resolved_path == NULL && result == NULL)
        free(_resolved_path);

    // set an empty string on resolved_path in case of NULL result
    if (resolved_path != NULL && result == NULL)
        resolved_path[0] = 0;

    return result;
}