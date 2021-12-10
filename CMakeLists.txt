cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(zstr)

if(${CMAKE_VERSION} VERSION_LESS 3.13)
  message(WARNING "Interface libraries are not well supported before cmake 3.13, you will need to include within the parent CMakeLists.txt: INCLUDE_DIRECTORIES(zstr/src)") 
endif()

# zlib is required
find_package(ZLIB 1.2.3 REQUIRED)

add_library(zstr INTERFACE)
target_include_directories(zstr INTERFACE src ${ZLIB_INCLUDE_DIRS})
target_link_libraries(zstr INTERFACE ${ZLIB_LIBRARIES})

message(STATUS "Adding zstr and ZLIB for ${PROJECT_NAME}: includes: ${CMAKE_CURRENT_SOURCE_DIR}/src ${ZLIB_INCLUDE_DIRS} and libraries: ${ZLIB_LIBRARIES}")

