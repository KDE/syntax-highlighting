cmake_minimum_required(VERSION 3.0)

set(KF5_VERSION "5.35.0")
project(KSyntaxHighlighting VERSION ${KF5_VERSION})

find_package(ECM 5.35.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(CMakePackageConfigHelpers)
include(ECMPoQmTools)
include(ECMQtDeclareLoggingCategory)
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)
include(ECMMarkNonGuiExecutable)


ecm_setup_version(PROJECT
    VARIABLE_PREFIX SyntaxHighlighting
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/ksyntaxhighlighting_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5SyntaxHighlightingConfigVersion.cmake"
)

#
# Dependencies
#
set(REQUIRED_QT_VERSION "5.6.0")
find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED COMPONENTS Core Gui Network Test)
find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE QUIET OPTIONAL_COMPONENTS Widgets XmlPatterns)
set_package_properties(Qt5 PROPERTIES URL "http://qt-project.org/")
set_package_properties(Qt5Widgets PROPERTIES PURPOSE "Example application.")
set_package_properties(Qt5XmlPatterns PROPERTIES PURPOSE "Compile-time validation of syntax definition files.")

find_package(Perl REQUIRED)
set_package_properties(Perl PROPERTIES PURPOSE "Auto-generate PHP syntax definition files.")

#
# Translations
#
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

#
# Actually build the stuff
#
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(autotests)

#
# CMake package config file generation
#
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5SyntaxHighlighting")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5SyntaxHighlightingConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5SyntaxHighlightingConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/KF5SyntaxHighlightingConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/KF5SyntaxHighlightingConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT KF5SyntaxHighlightingTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE KF5SyntaxHighlightingTargets.cmake
        NAMESPACE KF5::)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ksyntaxhighlighting_version.h"
        DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF5}"
        COMPONENT Devel)
install(FILES org_kde_ksyntaxhighlighting.categories DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
