set(kate_syntax_srcs
   syntaxdefinition.cpp
)
qt5_add_resources(kate_syntax_srcs ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/kate/data/syntax-data.qrc)

add_library(KateSyntax SHARED ${kate_syntax_srcs})
generate_export_header(KateSyntax)
target_link_libraries(KateSyntax LINK_PUBLIC Qt5::Core)

install(TARGETS KateSyntax EXPORT KateSyntaxTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
