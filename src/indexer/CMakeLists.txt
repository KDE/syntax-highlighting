add_executable(katehighlightingindexer katehighlightingindexer.cpp)

if(Qt5XmlPatterns_FOUND)
  target_link_libraries(katehighlightingindexer Qt5::XmlPatterns)
else()
  target_link_libraries(katehighlightingindexer Qt5::Core)
endif()
