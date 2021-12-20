if(TARGET Qt${QT_MAJOR_VERSION}::Widgets)
    add_executable(codeeditor codeeditor.cpp main.cpp)
    target_link_libraries(codeeditor Qt${QT_MAJOR_VERSION}::Widgets KF5SyntaxHighlighting)
endif()
