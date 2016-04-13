set(kate_syntax_srcs
   syntaxdefinition.cpp
)

add_library(KateSyntax SHARED ${kate_syntax_srcs})
generate_export_header(KateSyntax)
target_link_libraries(KateSyntax LINK_PUBLIC Qt5::Core)

install(TARGETS KateSyntax EXPORT KateSyntaxTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
