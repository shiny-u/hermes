#=============================================================================
#
# CMake configuration file for the hermes project
#
#=============================================================================

cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(hermes LANGUAGES CXX)

# Prevent in-source build
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message( FATAL_ERROR  "In-source build is not possible and not recommended. Choose an empty directory for build output.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

set(hermes_is_toplevel_project FALSE)
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(hermes_is_toplevel_project TRUE)
endif()

# Languages requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

# Version informations
set(hermes_VERSION_MAJOR   "0")
set(hermes_VERSION_MINOR   "1")
set(hermes_VERSION_PATCH   "0")
set(hermes_VERSION
  "${hermes_VERSION_MAJOR}.${hermes_VERSION_MINOR}.${hermes_VERSION_PATCH}")

# Cmake scripts
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#=============================================================================
# Retrieving the current Git revision
#=============================================================================
set(PRE_CONFIGURE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/hermes/version.h.in CACHE FILEPATH "" FORCE)
set(POST_CONFIGURE_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/hermes/version.h CACHE FILEPATH "" FORCE)
set(GIT_WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE FILEPATH "" FORCE)
set(GIT_STATE_FILE ${CMAKE_CURRENT_BINARY_DIR}/git-state-hash CACHE FILEPATH "" FORCE)
include(cmake/git_watcher.cmake)

if (NOT EXISTS ${POST_CONFIGURE_FILE})
    configure_file(${PRE_CONFIGURE_FILE} ${POST_CONFIGURE_FILE})
endif()

# Hermes options
option(HERMES_BUILD_TESTS "Tests building." ${hermes_is_toplevel_project})
option(HERMES_ENABLE_PSQL "Build PostgreSQL serializer" OFF)
option(HERMES_USE_DUNITS "Use DUNITS lib to check units" OFF)

# hermes depends on fmt library
include(cmake/URL.conf.cmake)
include(Add_fmt)
include(Add_spdlog)
include(Add_Eigen3)
if (${HERMES_ENABLE_PSQL})
    include(Add_pqxx)
endif()
if (HERMES_USE_DUNITS)
    include(Add_dunits)
endif ()

# For Windows (exposing public API)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_subdirectory(src)
add_dependencies(hermes check_git_hermes) # For git_watcher to fetch git informations before effective build

# Adding tests
if (HERMES_BUILD_TESTS)
    include(Add_googletest)
    enable_testing()
    add_subdirectory(tests)
endif()
