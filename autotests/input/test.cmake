# comment

#[[.rst:
MyModule
--------

bla bla
#]]

#.rst:
# ATTENTION
# alternative doc comment
#

#[=====[.rst:

3rd form

]=====]

set(CMAKE_AUTOMOC ON)
if (POLICY CMP042)
    target_link_libraries(myTaget SHARED Qt5::Core)
else()
    add_executable(myTaget ${some_var})
endif()

macro(my_macro arg1)
    foreach(arg ${ARGN})
    endforeach()
endmacro()
