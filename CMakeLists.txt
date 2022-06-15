project(AppImageExecWrapper)
cmake_minimum_required(VERSION 3.2)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

if (${CMAKE_BUILD_TYPE} MATCHES Profile)
    message(STATUS "Coverage and profile build flags enabled")
    set(CMAKE_C_FLAGS "-fprofile-arcs -ftest-coverage")
endif (${CMAKE_BUILD_TYPE} MATCHES Profile)

include(cmake/get_libconfig.cmake)
add_subdirectory(src)

if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif ()