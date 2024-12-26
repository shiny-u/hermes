include(FetchContent)

FetchContent_Declare(googletest
  GIT_REPOSITORY ${googletest_URL}
  GIT_TAG ${googletest_TAG}
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()
