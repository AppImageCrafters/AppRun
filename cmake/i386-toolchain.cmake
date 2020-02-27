# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /usr/bin/gcc)
set(CMAKE_C_COMPILER_ARG1 "-m32")
SET(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_COMPILER_ARG1 "-m32")

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH /usr/lib/i386-linux-gnu)
set(CMAKE_IGNORE_PATH /usr/lib/x86_64-linux-gnu/ /usr/lib/x86_64-linux-gnu/lib/)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
