set(kate_syntax_srcs
    abstracthighlighter.cpp
    context.cpp
    contextswitch.cpp
    downloader.cpp
    format.cpp
    htmlhighlighter.cpp
    keywordlist.cpp
    matchresult.cpp
    rule.cpp
    syntaxdefinition.cpp
    syntaxhighlighter.cpp
    syntaxrepository.cpp
    theme.cpp
)
qt5_add_resources(kate_syntax_srcs ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/kate/data/syntax-data.qrc)

add_library(KateSyntax SHARED ${kate_syntax_srcs})
generate_export_header(KateSyntax)
set_target_properties(KateSyntax PROPERTIES
    VERSION ${SyntaxHighlighting_VERSION_STRING}
    SOVERSION ${SyntaxHighlighting_SOVERSION}
    EXPORT_NAME SyntaxHighlighting
)
target_include_directories(KateSyntax PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")
target_link_libraries(KateSyntax LINK_PUBLIC Qt5::Gui LINK_PRIVATE Qt5::Network)

install(TARGETS KateSyntax EXPORT KF5SyntaxHighlightingTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
    syntaxrepository.h
    syntaxdefinition.h
    abstracthighlighter.h
    syntaxhighlighter.h
    theme.h
    ${CMAKE_CURRENT_BINARY_DIR}/katesyntax_export.h
    DESTINATION ${INCLUDE_INSTALL_DIR})
