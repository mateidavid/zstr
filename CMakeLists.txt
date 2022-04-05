cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(zstr)

if(${CMAKE_VERSION} VERSION_LESS 3.13)
  message(WARNING
    "Interface libraries are not well supported before cmake 3.13, you will "
    "need to manually add 'zstr/src' to your include directories. The simplest "
    "way is to add the following line within the parent CMakeLists.txt:\n"
    "include_directories(zstr/src)"
  )
endif()

# zlib is required
find_package(ZLIB 1.2.3 REQUIRED) # defines imported target ZLIB::ZLIB
message(STATUS "zstr - found ZLIB (version: ${ZLIB_VERSION_STRING})")

add_library(zstr INTERFACE)
target_include_directories(zstr INTERFACE src ${ZLIB_INCLUDE_DIRS})
target_link_libraries(zstr INTERFACE ${ZLIB_LIBRARIES})


message(STATUS
  "zstr - added INTERFACE target 'zstr'
          includes: ${PROJECT_SOURCE_DIR}/src  ${ZLIB_INCLUDE_DIRS}
          libraries: ${ZLIB_LIBRARIES}"
)
