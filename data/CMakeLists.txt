# create a directory for generated definitions
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated/syntax)

macro(generate_syntax_definition generator targetFile srcFile)
    add_custom_target(
        ${targetFile} ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generated/syntax/${targetFile}
    )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/syntax/${targetFile}
        COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generators/${generator}
                ${CMAKE_CURRENT_SOURCE_DIR}/syntax/${srcFile}
                ${CMAKE_CURRENT_BINARY_DIR}/generated/syntax/${targetFile}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generators/${generator}
                ${CMAKE_CURRENT_SOURCE_DIR}/syntax/${srcFile}
                ${ARGN}
        VERBATIM
    )
    set(gen_defs ${gen_defs} ${CMAKE_CURRENT_BINARY_DIR}/generated/syntax/${targetFile})
endmacro()

macro(generate_syntax_definition_with_html syntax mergedSyntax)
    generate_syntax_definition(generate-html.pl ${mergedSyntax}-${syntax}.xml ${mergedSyntax}.xml ${CMAKE_CURRENT_SOURCE_DIR}/syntax/${syntax}.xml)
endmacro()

# generate PHP definitions
generate_syntax_definition_with_html(php html)
generate_syntax_definition_with_html(php css)
generate_syntax_definition_with_html(php javascript)
generate_syntax_definition_with_html(php javascript-react)
generate_syntax_definition_with_html(php typescript)
generate_syntax_definition_with_html(php mustache)

# generate Twig definitions
generate_syntax_definition_with_html(twig html)
generate_syntax_definition_with_html(twig css)
generate_syntax_definition_with_html(twig javascript)
generate_syntax_definition_with_html(twig javascript-react)
generate_syntax_definition_with_html(twig typescript)

# generate DoxygenLua definition
generate_syntax_definition(generate-doxygenlua.pl doxygenlua.xml doxygen.xml)

# find all definitions
file(GLOB src_defs "${CMAKE_CURRENT_SOURCE_DIR}/syntax/*.xml")
set(defs ${src_defs} ${gen_defs})

# object library to make cross-folder dependencies work
add_library(SyntaxHighlightingData OBJECT)

# theme data resource
target_sources(SyntaxHighlightingData PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/themes/theme-data.qrc)

# generate the resource file
set(compressed_xml_dir ${CMAKE_CURRENT_BINARY_DIR}/generated/compressed-syntax)
set(qrc_xml_file ${CMAKE_CURRENT_BINARY_DIR}/syntax-data-xml.qrc)
set(qrc_file ${CMAKE_CURRENT_BINARY_DIR}/syntax-data.qrc)
set(qrc_xml_body "")
set(qrc_body "")
foreach(def ${defs})
    get_filename_component(def_name ${def} NAME)
    string(APPEND qrc_body "<file alias=\"${def_name}\">${compressed_xml_dir}/${def_name}</file>\n")
    string(APPEND qrc_xml_body "<file>${def}</file>\n")
    set(compressedDefs ${compressedDefs} ${compressed_xml_dir}/${def_name})
endforeach()
write_file(${qrc_xml_file} "<!DOCTYPE RCC>\n<RCC>${qrc_xml_body}</RCC>")
set(SYNTAX_DATA_QRC_FILES_STRING ${qrc_body})
configure_file(syntax-data.qrc.in ${qrc_file} @ONLY)

# compress xml files
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${compressed_xml_dir})

# generate the index file
add_custom_target(katesyntax DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax")
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax"
    COMMAND katehighlightingindexer
        "${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax"
        "${CMAKE_CURRENT_SOURCE_DIR}/schema/language.xsd"
        "${qrc_xml_file}"
        "${compressed_xml_dir}"
    DEPENDS katehighlightingindexer
        ${defs}
        ${qrc_file}
        ${CMAKE_CURRENT_SOURCE_DIR}/schema/language.xsd
)

# do we want syntax files bundled in the library?
if (QRC_SYNTAX)
    # generate the qrc file manually, to make dependencies on generated files work...
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp"
        COMMAND Qt6::rcc --name syntax_data -o "${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp" "${qrc_file}"
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax
    )
    set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp" PROPERTIES SKIP_AUTOMOC ON)

    target_sources(SyntaxHighlightingData PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/qrc_syntax-data.cpp)
else()
    add_dependencies(SyntaxHighlightingData katesyntax)
    # install the syntax files as normal files into the prefix
    install (FILES "${CMAKE_CURRENT_BINARY_DIR}/index.katesyntax" ${compressedDefs} DESTINATION ${KDE_INSTALL_DATADIR}/org.kde.syntax-highlighting/syntax-bundled)
endif()

# set PIC to allow use in static and shared libs
# this needs some more recent CMake than generally required
set_property(TARGET SyntaxHighlightingData PROPERTY POSITION_INDEPENDENT_CODE 1)
if(NOT ${CMAKE_VERSION} VERSION_LESS "3.13.0")
    target_link_libraries(SyntaxHighlightingData PRIVATE Qt6::Core)
endif()
