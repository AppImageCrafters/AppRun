set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i386)

set(CMAKE_C_FLAGS "-m32")
set(CMAKE_CXX_FLAGS "-m32")

# CMAKE_SHARED_LINKER_FLAGS, CMAKE_STATIC_LINKER_FLAGS etc. must not be set, but CMAKE_EXE_LINKER_FLAGS is necessary
set(CMAKE_EXE_LINKER_FLAGS "-m32")

set(DEPENDENCIES_CFLAGS "-m32")
set(DEPENDENCIES_CPPFLAGS "-m32")
set(DEPENDENCIES_LDFLAGS "-m32")

# host = target system
# build = build system
# both must be specified
set(EXTRA_CONFIGURE_FLAGS "--host=i686-pc-linux-gnu" "--build=x86_64-pc-linux-gnu")

# may help with some rare issues
set(CMAKE_PREFIX_PATH /usr/lib/i386-linux-gnu)

# makes sure that at least on Ubuntu pkg-config will search for the :i386 packages
set(ENV{PKG_CONFIG_PATH} /usr/lib/i386-linux-gnu/pkgconfig/)