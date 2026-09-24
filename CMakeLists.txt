cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(zstr LANGUAGES CXX)

# -- locate zlib

find_package(ZLIB 1.2.3 REQUIRED) # defines imported target ZLIB::ZLIB
message(STATUS "zstr - found ZLIB (version: ${ZLIB_VERSION_STRING})")

# -- add target

add_library(zstr INTERFACE)
add_library(zstr::zstr ALIAS zstr)

# -- set target properties

target_include_directories(zstr INTERFACE "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(zstr INTERFACE ZLIB::ZLIB)
target_compile_features(zstr INTERFACE cxx_std_11) # require c++11 flag

# -- set cache variables

# NOTE: these vars are mostly useful to people using cmake < 3.13
set(ZSTR_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src;${ZLIB_INCLUDE_DIRS}" CACHE PATH "" FORCE)
set(ZSTR_LIBRARIES "${ZLIB_LIBRARIES}" CACHE PATH "" FORCE)

# -- print target summary

message(STATUS
  "zstr - added INTERFACE target 'zstr::zstr'
          includes : ${ZSTR_INCLUDE_DIRS}
          libraries: ZLIB::ZLIB
          features : cxx_std_11"
)
