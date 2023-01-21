if(TARGET Qt6::Widgets)
    add_executable(minimal main.cpp)
    target_link_libraries(minimal Qt6::Widgets KF6SyntaxHighlighting)
endif()
