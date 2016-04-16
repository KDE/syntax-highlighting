#include <stdio.h>
#include <stdint.h>

#define SOME_VAR 1
#ifdef SOME_VAR

static uint64_t intWithSuffix = 42ull;
static int octNum = 07232;
static int invalidOctNum = 09231;
static uint64_t hexNum = 0xDEADBEEF42;
static uint64_t invalidHexNum = 0xGLDFKG;
static char binNum = 0b0101010;

static float floatQualifier = 23.123f;

//BEGIN region
// TODO comment FIXME comment ### comment
//END region

#wrong

#endif
