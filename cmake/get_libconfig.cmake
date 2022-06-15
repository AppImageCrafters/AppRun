cmake_minimum_required(VERSION 3.11)
include(ExternalProject)


# Get the libconfig library
externalproject_add(
    libconfig.git
    GIT_REPOSITORY https://github.com/hyperrealm/libconfig.git
    GIT_TAG v1.7.3
    GIT_SHALLOW On
    UPDATE_DISCONNECTED On
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=/usr
    -DBUILD_EXAMPLES=Off
    -DBUILD_TESTS=Off
    -DBUILD_SHARED_LIBS=Off
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

ExternalProject_Get_Property(libconfig.git install_dir)
include_directories(${install_dir}/include)


add_library(libconfig STATIC IMPORTED)
set_property(TARGET libconfig PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libconfig.a)

add_library(libconfig++ STATIC IMPORTED)
set_property(TARGET libconfig++ PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libconfig++.a)

add_dependencies(libconfig libconfig.git)
add_dependencies(libconfig++ libconfig.git)