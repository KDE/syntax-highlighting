#include <stdio.h>
#include "stdint.h"

#define SOME_VAR 1
#ifdef SOME_VAR

#define MULTILINE_MACRO one \
two \
three

static uint64_t intWithSuffix = 42ull;
static int octNum = 07232;
static int invalidOctNum = 09231;
static uint64_t hexNum = 0xDEADBEEF42;
static uint64_t invalidHexNum = 0xGLDFKG;
static char binNum = 0b0101010;

static float floatQualifier = 23.123f;

static const char c1 = 'c';
static const char c2 = '\n';
static const char c2a = '\020';
static const char c2b = '\x10';
static const char c2c = '\'';
static const char c2d = '\\';
static const char* c3  = "string";
static const char* c4 = "\"string\n\t\012\x12\"";
static const char* c5 = "multiline \
    string";

//BEGIN region
// TODO comment FIXME comment ### comment
//END region

#wrong

#endif
