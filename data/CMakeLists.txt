# find all definitions
file(GLOB src_defs "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
set(defs
    ${src_defs}
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
file(APPEND ${qrc_file} "</qresource>\n")
file(APPEND ${qrc_file} "</RCC>\n")
