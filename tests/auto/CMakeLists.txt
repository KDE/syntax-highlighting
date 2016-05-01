configure_file(test-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/test-config.h)

add_executable(syntaxrepository_test syntaxrepository_test.cpp)
add_test(NAME syntaxrepository_test COMMAND syntaxrepository_test)
target_link_libraries(syntaxrepository_test KateSyntax Qt5::Test)

add_executable(testhighlighter_test testhighlighter.cpp)
add_test(NAME testhighlighter_test COMMAND testhighlighter_test)
target_link_libraries(testhighlighter_test KateSyntax Qt5::Test)

add_executable(htmlhighlighter_test htmlhighlighter_test.cpp)
add_test(NAME htmlhighlighter_test COMMAND htmlhighlighter_test)
target_link_libraries(htmlhighlighter_test KateSyntax Qt5::Test)

add_executable(highlighter_benchmark highlighter_benchmark.cpp)
add_test(NAME highlighter_benchmark COMMAND highlighter_benchmark)
target_link_libraries(highlighter_benchmark KateSyntax Qt5::Test)
