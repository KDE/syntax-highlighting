set(kate_syntax_srcs
    abstracthighlighter.cpp
    context.cpp
    htmlhighlighter.cpp
    keywordlist.cpp
    rule.cpp
    syntaxdefinition.cpp
    syntaxrepository.cpp
)
qt5_add_resources(kate_syntax_srcs ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/kate/data/syntax-data.qrc)

add_library(KateSyntax SHARED ${kate_syntax_srcs})
generate_export_header(KateSyntax)
target_include_directories(KateSyntax PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR};>")
target_link_libraries(KateSyntax LINK_PUBLIC Qt5::Core)

install(TARGETS KateSyntax EXPORT KateSyntaxTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
