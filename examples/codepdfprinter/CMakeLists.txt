if(TARGET Qt${QT_MAJOR_VERSION}::Widgets AND TARGET Qt${QT_MAJOR_VERSION}::PrintSupport)
    add_executable(codepdfprinter codepdfprinter.cpp main.cpp)
    target_link_libraries(codepdfprinter Qt${QT_MAJOR_VERSION}::Widgets Qt${QT_MAJOR_VERSION}::PrintSupport KF5SyntaxHighlighting)
endif()
