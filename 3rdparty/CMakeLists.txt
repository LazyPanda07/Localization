cmake_minimum_required(VERSION 3.27.0)

project(3rdparty)

set(JSON_TAG v2.6.1)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/Localization/_deps/)

get_directory_property(HAS_PARENT PARENT_DIRECTORY)

include(FetchContent)

FetchContent_Declare(
  JSON
  URL https://github.com/LazyPanda07/JSON/archive/refs/tags/${JSON_TAG}.zip
)

FetchContent_MakeAvailable(JSON)

if(HAS_PARENT)
  set(LOCALIZATION_JSON_INCLUDE ${json_SOURCE_DIR}/src/ PARENT_SCOPE)
endif()
