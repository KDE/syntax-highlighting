set(kate_syntax_srcs
    abstracthighlighter.cpp
    context.cpp
    contextswitch.cpp
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
    VERSION ${KATESYNTAX_VERSION}
    SOVERSION ${KATESYNTAX_VERSION}
)
target_include_directories(KateSyntax PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")
target_link_libraries(KateSyntax LINK_PUBLIC Qt5::Gui)

install(TARGETS KateSyntax EXPORT KateSyntaxTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
