include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)
configure_file(test-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/test-config.h)

add_executable(syntaxrepository_test syntaxrepository_test.cpp)
add_test(NAME syntaxrepository_test COMMAND syntaxrepository_test)
target_link_libraries(syntaxrepository_test KateSyntax Qt5::Test)

add_executable(htmlhighlighter_test htmlhighlighter_test.cpp)
add_test(NAME htmlhighlighter_test COMMAND htmlhighlighter_test)
target_link_libraries(htmlhighlighter_test KateSyntax Qt5::Test)
