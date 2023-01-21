if(TARGET Qt6::Widgets)
    add_executable(codeeditor codeeditor.cpp main.cpp)
    target_link_libraries(codeeditor Qt6::Widgets KF6SyntaxHighlighting)
endif()
