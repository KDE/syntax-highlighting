add_library(KF6SyntaxHighlighting)

ecm_create_qm_loader(syntax_highlighting_QM_LOADER syntaxhighlighting5_qt)

target_sources(KF6SyntaxHighlighting PRIVATE
    abstracthighlighter.cpp
    context.cpp
    contextswitch.cpp
    definitiondownloader.cpp
    highlightingdata.cpp
    foldingregion.cpp
    format.cpp
    htmlhighlighter.cpp
    ansihighlighter.cpp
    keywordlist.cpp
    rule.cpp
    definition.cpp
    repository.cpp
    state.cpp
    syntaxhighlighter.cpp
    theme.cpp
    wildcardmatcher.cpp
    themedata.cpp
    worddelimiters.cpp
    ${syntax_highlighting_QM_LOADER}
    $<TARGET_OBJECTS:SyntaxHighlightingData>
)
ecm_qt_declare_logging_category(KF6SyntaxHighlighting
    HEADER ksyntaxhighlighting_logging.h
    IDENTIFIER KSyntaxHighlighting::Log
    CATEGORY_NAME kf.syntaxhighlighting
    OLD_CATEGORY_NAMES org.kde.ksyntaxhighlighting
    DESCRIPTION "Syntax Highlighting"
    EXPORT KSYNTAXHIGHLIGHTING
)

ecm_generate_export_header(KF6SyntaxHighlighting
    BASE_NAME KSyntaxHighlighting
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)
set_target_properties(KF6SyntaxHighlighting PROPERTIES
    VERSION ${SyntaxHighlighting_VERSION}
    SOVERSION ${SyntaxHighlighting_SOVERSION}
    EXPORT_NAME SyntaxHighlighting
)

target_link_libraries(KF6SyntaxHighlighting
    PUBLIC
        Qt6::Gui
    PRIVATE
        Qt6::Network
)

set(Forwarding_Header_Names
    AbstractHighlighter
    Definition
    DefinitionDownloader
    FoldingRegion
    Format
    Repository
    State
    SyntaxHighlighter
    Theme
    WildcardMatcher
)

ecm_generate_headers(CamelCase_HEADERS
    HEADER_NAMES ${Forwarding_Header_Names}
    REQUIRED_HEADERS SyntaxHighlighting_HEADERS
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/KSyntaxHighlighting/KSyntaxHighlighting
)

target_include_directories(KF6SyntaxHighlighting
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KSyntaxHighlighting>"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/KSyntaxHighlighting;>"
)

install(TARGETS KF6SyntaxHighlighting EXPORT KF6SyntaxHighlightingTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${CamelCase_HEADERS}
    ${SyntaxHighlighting_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/ksyntaxhighlighting_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KSyntaxHighlighting/KSyntaxHighlighting
)

if(BUILD_QCH)
    ecm_add_qch(
        KF6SyntaxHighlighting_QCH
        NAME KSyntaxHighlighting
        BASE_NAME KF6SyntaxHighlighting
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${SyntaxHighlighting_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Core_QCH
            Qt5Gui_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KSYNTAXHIGHLIGHTING_EXPORT
            KSYNTAXHIGHLIGHTING_DEPRECATED
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()
ecm_generate_pri_file(
    BASE_NAME KSyntaxHighlighting LIB_NAME
    KF6SyntaxHighlighting
    DEPS "gui"
    FILENAME_VAR PRI_FILENAME
    INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KSyntaxHighlighting
)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
