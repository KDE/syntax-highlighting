cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "6.10.0")
project(KSyntaxHighlighting VERSION ${KF_VERSION})

find_package(ECM 6.10.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDEGitCommitHooks)

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)
include(ECMPoQmTools)
include(ECMQtDeclareLoggingCategory)
include(ECMMarkNonGuiExecutable)
include(ECMAddQch)
include(ECMOptionalAddSubdirectory)
include(ECMGenerateExportHeader)
include(ECMDeprecationSettings)
include(ECMQmlModule)

set(ksyntaxhighlighting_version_header "${CMAKE_CURRENT_BINARY_DIR}/src/lib/ksyntaxhighlighting_version.h")
ecm_setup_version(PROJECT
    VARIABLE_PREFIX KSYNTAXHIGHLIGHTING
    VERSION_HEADER "${ksyntaxhighlighting_version_header}"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF6SyntaxHighlightingConfigVersion.cmake"
    SOVERSION 6
)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

#
# Dependencies
#
set(REQUIRED_QT_VERSION 6.6.0)
find_package(Qt6 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED COMPONENTS Core Network Test)
option(KSYNTAXHIGHLIGHTING_USE_GUI "Build components depending on QtGui" ON)
if(KSYNTAXHIGHLIGHTING_USE_GUI)
    find_package(Qt6 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED COMPONENTS Gui)
endif()
find_package(Qt6 ${REQUIRED_QT_VERSION} NO_MODULE QUIET OPTIONAL_COMPONENTS PrintSupport Widgets Quick)
set_package_properties(Qt6 PROPERTIES URL "http://qt-project.org/")
set_package_properties(Qt6Widgets PROPERTIES PURPOSE "Example application.")
set_package_properties(Qt6PrintSupport PROPERTIES PURPOSE "Example application.")
set_package_properties(Qt6Quick PROPERTIES PURPOSE "QtQuick bindings.")

# https://xerces.apache.org/index.html for validation
find_package (XercesC)
set_package_properties(XercesC PROPERTIES PURPOSE "Compile-time validation of syntax definition files.")

find_package(Perl REQUIRED)
set_package_properties(Perl PROPERTIES PURPOSE "Auto-generate PHP syntax definition files.")

#
# allow to install the "differently" licensed syntax xml files instead of putting them in a QRC and link them in
#
option(QRC_SYNTAX "Bundle the syntax definition files inside the library as resources" ON)
add_feature_info(SYNTAX_RESOURCE ${QRC_SYNTAX} "Bundle the syntax definition files inside the library as resources")

#
# allow to turn of lookup for syntax files and themes via QStandardPaths
#
option(NO_STANDARD_PATHS "Skip lookup of syntax and theme definitions in QStandardPaths locations" OFF)
add_feature_info(FEATURE_NO_STANDARD_PATHS ${NO_STANDARD_PATHS} "Skip lookup of syntax and theme definitions in QStandardPaths locations")

#
# API documentation
#
option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

#
# Translations
#
ecm_install_po_files_as_qm(poqm)

# tell the framework if it shall use the syntax files from the resource
if (QRC_SYNTAX)
    add_definitions(-DHAS_SYNTAX_RESOURCE)
endif()

# skip standard paths?
if (NO_STANDARD_PATHS)
    add_definitions(-DNO_STANDARD_PATHS)
endif()

ecm_set_disabled_deprecation_versions(
    QT 6.8.0
)

#
# Actually build the stuff
#
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(data)
add_subdirectory(src)
if(TARGET Qt6::Gui)
    add_subdirectory(examples)
    if (BUILD_TESTING)
        add_subdirectory(autotests)
    endif()
endif()

#
# CMake package config file generation
#
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF6SyntaxHighlighting")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF6SyntaxHighlighting_QCH
        FILE KF6SyntaxHighlightingQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF6SyntaxHighlightingQchTargets.cmake\")")
endif()

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF6SyntaxHighlightingConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF6SyntaxHighlightingConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/KF6SyntaxHighlightingConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/KF6SyntaxHighlightingConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

if(TARGET KF6SyntaxHighlighting)
    install(EXPORT KF6SyntaxHighlightingTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE KF6SyntaxHighlightingTargets.cmake
        NAMESPACE KF6::)
endif()

install(FILES "${ksyntaxhighlighting_version_header}"
        DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF}/KSyntaxHighlighting"
        COMPONENT Devel)

include(ECMFeatureSummary)
ecm_feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# add target to update kate-editor.org syntax page + update site
add_custom_target(update_kate_editor_org
    COMMAND "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/utils/update-kate-editor-org.pl" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}")

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
