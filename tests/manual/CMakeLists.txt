include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)
add_executable(qsyntaxhighlighter qsyntaxhighlighter.cpp)
target_link_libraries(qsyntaxhighlighter Qt5::Widgets KateSyntax)
