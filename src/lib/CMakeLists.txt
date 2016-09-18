ecm_create_qm_loader(syntax_highlighting_QM_LOADER syntaxhighlighting5_qt)

set(syntax_highlighting_srcs
    abstracthighlighter.cpp
    context.cpp
    contextswitch.cpp
    downloader.cpp
    format.cpp
    htmlhighlighter.cpp
    keywordlist.cpp
    matchresult.cpp
    rule.cpp
    definition.cpp
    repository.cpp
    state.cpp
    syntaxhighlighter.cpp
    theme.cpp
    wildcardmatcher.cpp
    ${syntax_highlighting_QM_LOADER}
)
ecm_qt_declare_logging_category(syntax_highlighting_srcs
    HEADER syntaxhighlighting_logging.h
    IDENTIFIER SyntaxHighlighting::Log
    CATEGORY_NAME org.kde.syntax-highlighting
)

add_library(KF5SyntaxHighlighting SHARED ${syntax_highlighting_srcs})
generate_export_header(KF5SyntaxHighlighting)
set_target_properties(KF5SyntaxHighlighting PROPERTIES
    VERSION ${SyntaxHighlighting_VERSION_STRING}
    SOVERSION ${SyntaxHighlighting_SOVERSION}
    EXPORT_NAME SyntaxHighlighting
)
target_include_directories(KF5SyntaxHighlighting PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")
target_link_libraries(KF5SyntaxHighlighting LINK_PUBLIC Qt5::Gui LINK_PRIVATE Qt5::Network SyntaxHighlightingData)

ecm_generate_headers(SyntaxHighlighting_HEADERS
    HEADER_NAMES
        AbstractHighlighter
        Definition
        Format
        Repository
        State
        SyntaxHighlighter
        Theme
    REQUIRED_HEADERS SyntaxHighlighting_HEADERS
)

install(TARGETS KF5SyntaxHighlighting EXPORT KF5SyntaxHighlightingTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
    ${SyntaxHighlighting_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/kf5syntaxhighlighting_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/SyntaxHighlighting)

ecm_generate_pri_file(
    BASE_NAME SyntaxHighlighting LIB_NAME
    KF5SyntaxHighlighting
    DEPS "gui"
    FILENAME_VAR PRI_FILENAME
    INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/SyntaxHighlighting
)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
