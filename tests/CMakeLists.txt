if(Qt5Widgets_FOUND)
    add_executable(qsyntaxhighlighter qsyntaxhighlighter.cpp)
    target_link_libraries(qsyntaxhighlighter Qt5::Widgets KF5SyntaxHighlighting)
endif()
