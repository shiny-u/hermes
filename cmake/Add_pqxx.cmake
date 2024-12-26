include(FetchContent)

FetchContent_Declare(pqxx
  GIT_REPOSITORY https://github.com/jtv/libpqxx
  GIT_TAG 7.7.4
)
FetchContent_GetProperties(pqxx)
if(NOT pqxx_POPULATED)
  message(STATUS "******* FETCHING pqxx dependency from ${PROJECT_NAME} (requested version: ${pqxx_TAG}) *******")
  set(BUILD_TEST OFF CACHE BOOL "")
  FetchContent_Populate(pqxx)
  add_subdirectory(${pqxx_SOURCE_DIR} ${pqxx_BINARY_DIR})
endif()
