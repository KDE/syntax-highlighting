project(KateSyntax)

cmake_minimum_required(VERSION 2.8.12)
if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(NOT DEFINED CMAKE_INSTALL_RPATH_USE_LINK_PATH)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()
if(NOT DEFINED CMAKE_MACOSX_RPATH)
    set(CMAKE_MACOSX_RPATH TRUE)
endif()

include(FeatureSummary)
include(GenerateExportHeader)

enable_testing()

#
# Version setup
#
set(KATESYNTAX_VERSION_MAJOR "0")
set(KATESYNTAX_VERSION_MINOR "0")
set(KATESYNTAX_VERSION_PATCH "1")
set(KATESYNTAX_VERSION "${KATESYNTAX_VERSION_MAJOR}.${KATESYNTAX_VERSION_MINOR}.${KATESYNTAX_VERSION_PATCH}")
set(KATESYNTAX_VERSION_STRING "${KATESYNTAX_VERSION}")

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git)
  set_package_properties(Git PROPERTIES TYPE OPTIONAL PURPOSE "Determine exact build version.")
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE _git_revision
    )
    string(REGEX REPLACE "\n" "" _git_revision "${_git_revision}")
    set(KATESYNTAX_VERSION_STRING "${KATESYNTAX_VERSION_STRING} (revision: ${_git_revision})")
  endif()
endif()

configure_file(config-katesyntax-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-katesyntax-version.h)


#
# Compiler & linker settings
#
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

if(CMAKE_COMPILER_IS_GNUCXX OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -Werror=return-type -std=c++0x -pedantic")
endif()

if(CMAKE_SYSTEM_NAME MATCHES Linux OR CMAKE_SYSTEM_NAME STREQUAL GNU)
    if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined ${CMAKE_SHARED_LINKER_FLAGS}")
        set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined ${CMAKE_MODULE_LINKER_FLAGS}")
    endif()
endif()

#
# Dependencies
#
find_package(Qt5 NO_MODULE REQUIRED COMPONENTS Gui Test)
find_package(Qt5 NO_MODULE QUIET OPTIONAL_COMPONENTS Widgets)
add_definitions(-DQT_DEPRECATED_WARNINGS)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_NO_CAST_TO_ASCII -DQT_NO_URL_CAST_FROM_STRING -DQT_NO_CAST_FROM_ASCII)

set_package_properties(Qt5 PROPERTIES URL "http://qt-project.org/")

#
# Installation settings
#
set(BIN_INSTALL_DIR "bin")
set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")
set(INCLUDE_INSTALL_DIR "include/KateSyntax")
set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}/cmake/KateSyntax)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${BIN_INSTALL_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${LIB_INSTALL_DIR})

# set RPATH only when installing to a non-default location and not set in cache
if(NOT CMAKE_INSTALL_RPATH)
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemPlatformLibDir)
    list(FIND CMAKE_C_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemCLibDir)
    list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}" _isSystemCxxLibDir)
    if(${_isSystemPlatformLibDir} EQUAL -1 AND ${_isSystemCLibDir} EQUAL -1 AND ${_isSystemCxxLibDir} EQUAL -1)
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}")
    endif()
endif()

set(
    INSTALL_TARGETS_DEFAULT_ARGS
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel
)

#
# Actually build the stuff
#
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(tests)

#
# CMake package config file generation
#
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/KateSyntaxConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/KateSyntaxConfig.cmake
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS INCLUDE_INSTALL_DIR
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/KateSyntaxConfigVersion.cmake
    VERSION ${KATESYNTAX_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/KateSyntaxConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/KateSyntaxConfigVersion.cmake
    DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(EXPORT KateSyntaxTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KateSyntaxTarget.cmake)


feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
