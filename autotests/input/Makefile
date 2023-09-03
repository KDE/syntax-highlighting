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
	@echo "hello" \
  bla bla
	echo "hello" \
  bla bla

target3: ; @echo ’hello \
world’ ; echo "hello \
world"
	@echo "hello"

target4: ; echo ’hello \
world’ ; echo "hello \
world"
	echo "hello"

target5 \
target6 \
target7 &::
	@echo "hello"
# this should not break the rule
  # ifndef is a command
	ifndef

# String Substitution
VAR := $(ANOTHER_VAR:%=%+1)

# Variable Assignment
RELAXED ::= mello
rempl := x
foo := a b c
bar := foo
hello := $($(bar):a=$(rempl))
my??var=value
var??=value
var?=value
var? = value
var :::= value
var ::= value
var += value
var != value
  var = value\#nocomment
  var = value#comment
  var = value;value2
override var = a

define var :::=
bar
endef

override define var :::=
bar
endef

undefine var
override undefine var

export foo bar
export foo = bar
unexport foo bar

prog: private EXTRA_CFLAGS = -L/usr/local/lib
prog: CFLAGS = -g
prog: a.o b.o

# Conditionals
bar =
foo = $(bar)
ifdef bar
frobozz = yes
else ifdef foo
frobozz = no
endif

error_type := 0
check:
ifeq ($(error_type),0)
else ifeq ($(error_type),1)
else ifeq ($(error_type),2)
else ifeq ($(error_type),3)
endif

$(info "'")
$(info '"')
$(info $(??))

$(eval $(call Variable_rule,target/compile_flags,$${CXX} $$(CXXFLAGS)))
$(eval ${call Variable_rule,target/link_flags,$$(CXX) $$(LDFLAGS) $${LDLIBS}})
