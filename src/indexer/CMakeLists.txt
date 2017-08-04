if(CMAKE_CROSSCOMPILING AND KATEHIGHLIGHTINGINDEXER_EXECUTABLE)
    # cross build, we need the indexer supplied externally
    add_executable(katehighlightingindexer IMPORTED GLOBAL)
    set_target_properties(katehighlightingindexer PROPERTIES IMPORTED_LOCATION ${KATEHIGHLIGHTINGINDEXER_EXECUTABLE})
else()
    # host build
    add_executable(katehighlightingindexer katehighlightingindexer.cpp)
    if(Qt5XmlPatterns_FOUND)
        target_link_libraries(katehighlightingindexer Qt5::XmlPatterns)
    else()
        target_link_libraries(katehighlightingindexer Qt5::Core)
    endif()
endif()
