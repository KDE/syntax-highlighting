# generate PHP definitions
macro(generate_php_syntax_definition targetFile srcFile)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${targetFile}
        COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generators/generate-php.pl < ${CMAKE_CURRENT_SOURCE_DIR}/${srcFile} > ${CMAKE_CURRENT_BINARY_DIR}/${targetFile}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generators/generate-php.pl ${CMAKE_CURRENT_SOURCE_DIR}/${srcFile}
    )
endmacro()

generate_php_syntax_definition(javascript-php.xml javascript.xml)
generate_php_syntax_definition(css-php.xml css.xml)
generate_php_syntax_definition(html-php.xml html.xml)

# find all definitions
file(GLOB src_defs "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
set(defs
    ${src_defs}
    ${CMAKE_CURRENT_BINARY_DIR}/html-php.xml
    ${CMAKE_CURRENT_BINARY_DIR}/css-php.xml
    ${CMAKE_CURRENT_BINARY_DIR}/javascript-php.xml
)

# generate the resource file
set(qrc_file ${CMAKE_CURRENT_BINARY_DIR}/syntax-data.qrc)
file(REMOVE ${qrc_file})
file(APPEND ${qrc_file} "<!DOCTYPE RCC>\n")
file(APPEND ${qrc_file} "<RCC version=\"1.0\">\n")
file(APPEND ${qrc_file} "<qresource prefix=\"/syntaxhighlighting/syntax\">\n")
foreach(def ${defs})
    get_filename_component(def_name ${def} NAME)
    file(APPEND ${qrc_file} "<file alias=\"${def_name}\">${def}</file>\n")
endforeach()
file(APPEND ${qrc_file} "<file alias=\"index.katesyntax\">${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax</file>\n")
file(APPEND ${qrc_file} "</qresource>\n")
file(APPEND ${qrc_file} "</RCC>\n")

# generate the index file
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax"
  COMMAND katehighlightingindexer "${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax" "${CMAKE_CURRENT_SOURCE_DIR}/schema/language.xsd" "${CMAKE_CURRENT_BINARY_DIR}/syntax-data.qrc"
  DEPENDS ${defs}
)

# generate the qrc file manually, to make dependencies on generated files work...
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp"
    COMMAND ${Qt5Core_RCC_EXECUTABLE} --name syntax_data -o "${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp" "${CMAKE_CURRENT_BINARY_DIR}/syntax-data.qrc"
    DEPENDS ${defs} ${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax
)

# dummy static lib to make cross-folder dependencies work
add_library(SyntaxHighlightingData STATIC ${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp)
target_link_libraries(SyntaxHighlightingData LINK_PRIVATE Qt5::Core)
