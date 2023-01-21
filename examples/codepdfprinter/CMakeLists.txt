if(TARGET Qt6::Widgets AND TARGET Qt6::PrintSupport)
    add_executable(codepdfprinter codepdfprinter.cpp main.cpp)
    target_link_libraries(codepdfprinter Qt6::Widgets Qt6::PrintSupport KF6SyntaxHighlighting)
endif()
