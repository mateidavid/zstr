cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(zstr VERSION 1.2.0 LANGUAGES CXX)

# -- locate zlib

find_package(ZLIB 1.2.3 REQUIRED) # defines imported target ZLIB::ZLIB
message(STATUS "zstr - found ZLIB (version: ${ZLIB_VERSION_STRING})")

# -- add target

add_library(zstr INTERFACE)
add_library(zstr::zstr ALIAS zstr)

# -- set target properties

include(GNUInstallDirs)
target_include_directories(zstr INTERFACE 
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
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

include(CMakePackageConfigHelpers)
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/cmake/zstr")
install(TARGETS zstr
  EXPORT zstrTargets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/src/"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT zstrTargets
  FILE zstrTargets.cmake
  NAMESPACE zstr::
  DESTINATION ${CONFIG_INSTALL_DIR}
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/zstrConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
  ARCH_INDEPENDENT
)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/zstrConfig.cmake" "
include(CMakeFindDependencyMacro)
find_dependency(ZLIB 1.2.3 REQUIRED)
include(\"\${CMAKE_CURRENT_LIST_DIR}/zstrTargets.cmake\")
")
install(FILES 
  "${CMAKE_CURRENT_BINARY_DIR}/zstrConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/zstrConfigVersion.cmake"
  DESTINATION ${CONFIG_INSTALL_DIR}
)
