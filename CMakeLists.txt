# Example Audio Plugin CMakeLists.txt

# To get started on a new plugin, copy this entire folder (containing this file and C++ sources) to
# a convenient location, and then start making modifications.

# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.

cmake_minimum_required(VERSION 3.22)

# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(SIMPLE_DLY_PROJECT VERSION 0.0.1)

# Always use the newest C++ standard on green-field projects if possible.
set(CMAKE_CXX_STANDARD 23)

# I like to download the dependencies to the same folder as the project.
# If you want to install them system wide, set CPM_SOURCE_CACHE with the path to the dependencies
# either as an environment variable or pass it to the cmake script with -DCPM_SOURCE_CACHE=<path>.
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Downloads CPM if not already downloaded. CPM is an easy-to-use package manager nicely integrated with CMake.
include(cmake/cpm.cmake)

# This commands downloads AND configures JUCE. It sets up some variables, like JUCE_SOURCE_DIR.
CPMAddPackage(
    NAME JUCE
    GIT_TAG 7.0.5
    VERSION 7.0.5
    GITHUB_REPOSITORY juce-framework/JUCE
    SOURCE_DIR ${LIB_DIR}/juce
)


# Enables all warnings and treats warnings as errors.
# This needs to be added AFTER all the third-party dependencies.
if(MSVC)
    add_compile_options(/Wall /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Adds all the targets configured in the "plugin" folder.
add_subdirectory(plugin)
