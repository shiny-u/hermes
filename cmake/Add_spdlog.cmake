include(FetchContent)

FetchContent_Declare(spdlog
  GIT_REPOSITORY ${spdlog_URL}
  GIT_TAG ${spdlog_TAG}
)

FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
  message(STATUS "******* FETCHING spdlog dependency from ${PROJECT_NAME} (requested version: ${spdlog_TAG}) *******")
  FetchContent_Populate(spdlog)
  set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
  add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()

set_target_properties(spdlog PROPERTIES FOLDER deps)
