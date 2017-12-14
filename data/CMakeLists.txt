# generate PHP definitions
macro(generate_php_syntax_definition targetFile srcFile)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${targetFile}
        COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generators/generate-php.pl < ${CMAKE_CURRENT_SOURCE_DIR}/syntax/${srcFile} > ${CMAKE_CURRENT_BINARY_DIR}/${targetFile}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generators/generate-php.pl ${CMAKE_CURRENT_SOURCE_DIR}/syntax/${srcFile}
    )
endmacro()

generate_php_syntax_definition(javascript-php.xml javascript.xml)
generate_php_syntax_definition(css-php.xml css.xml)
generate_php_syntax_definition(html-php.xml html.xml)

# find all definitions
file(GLOB src_defs "${CMAKE_CURRENT_SOURCE_DIR}/syntax/*.xml")
set(defs
    ${src_defs}
    ${CMAKE_CURRENT_BINARY_DIR}/html-php.xml
    ${CMAKE_CURRENT_BINARY_DIR}/css-php.xml
    ${CMAKE_CURRENT_BINARY_DIR}/javascript-php.xml
)

# generate the resource file
set(qrc_file ${CMAKE_CURRENT_BINARY_DIR}/syntax-data.qrc)
set(qrc_body "")
foreach(def ${defs})
    get_filename_component(def_name ${def} NAME)
    string(APPEND qrc_body "<file alias=\"${def_name}\">${def}</file>\n")
endforeach()
set(SYNTAX_DATA_QRC_FILES_STRING ${qrc_body})
configure_file(syntax-data.qrc.in ${qrc_file} @ONLY)

# generate the index file
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax"
  COMMAND katehighlightingindexer "${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax" "${CMAKE_CURRENT_SOURCE_DIR}/schema/language.xsd" "${CMAKE_CURRENT_BINARY_DIR}/syntax-data.qrc"
  DEPENDS ${defs} ${CMAKE_CURRENT_SOURCE_DIR}/schema/language.xsd ${CMAKE_CURRENT_BINARY_DIR}/syntax-data.qrc
)

# generate the qrc file manually, to make dependencies on generated files work...
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp"
    COMMAND ${Qt5Core_RCC_EXECUTABLE} --name syntax_data -o "${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp" "${CMAKE_CURRENT_BINARY_DIR}/syntax-data.qrc"
    DEPENDS ${defs} ${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax
)
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp" PROPERTIES SKIP_AUTOMOC ON)

# dummy static lib to make cross-folder dependencies work
add_library(SyntaxHighlightingData STATIC ${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp)
target_link_libraries(SyntaxHighlightingData LINK_PRIVATE Qt5::Core)
