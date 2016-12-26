# comment

include Makefile.in
-include Makefile.doesntexist

.PHONY: all

all: target

foo = bar $(var) \
    $(baz) quux

ifeq ($(CC),gcc)
    bla=$(call func,param1,param2)
else ifeq "x" "y"
    $(error this seems wrong)
else
    libs=$(normal_libs)
endif

target1:
        error

target2: bla.cpp $@
	$(CC) bla.c -o bla.o
	$(CC) bla.o $< \
	    -o bla.exe
	@echo "hello"
