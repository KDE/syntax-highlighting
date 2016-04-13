include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)

add_executable(syntaxrepository_test syntaxrepository_test.cpp)
add_test(NAME syntaxrepository_test COMMAND syntaxrepository_test)
target_link_libraries(syntaxrepository_test KateSyntax Qt5::Test)
