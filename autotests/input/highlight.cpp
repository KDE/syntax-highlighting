#pragma once

#include <cassert>
#include <assert.h>
#include "assert.h"
#include "assert.hpp"
#include "path/assert.hpp"
#include PATH_IN_MACRO()

#define SOME_VAR 1
#ifdef SOME_VAR

#define MULTILINE_MACRO one \
two \
three

# define MULTILINE_MACRO_TEXT                        \
    /* NOTE The contents of macro is too green :D */ \
    char const s[] = "a\\b"                          \
    "c\nd"

# error dds
# warning dds
# line 2 "file.cpp"
# define A(x, y) x##y x#y
// OK(L, a) -> L"a"
# define OK(x, y) x###y
# define BAD(x, y) x####y

static int g_global;

template<class T, typename U, template<class> class = std::is_pointer>
struct class1
  : private std::vector<T>, public U
{
    class1()
        try
        : _member1(xxx)
    {}
        catch(...)
    {}

    ~class1()
    {}

    void foo() { return; }
    void foo() const { return; }
    void foo() noexcept { return; }
    void foo() const noexcept { return; }
    virtual void foo() const noexcept { return; }
    static void foo() { return; }
    constexpr static void foo() const
        noexcept(noexcept(std::is_pointer<U>::value)) override
    {
        xxx::template ttt<U>::type {};
        xxx.template get<U>();
        xxx.std::rdbuf();
        auto x = C<'a'> + y;
    }

    int operator->*(T (C::*m)(int));
    operator value_t ();

private:
protected:
public:
    value_type _member1; // NOTE internal ?
    value_type __internal;
    value_type internal__;
    value_type _M_internal;
    value_t member2_;
    value_type m_member3;

    static int s_static;
};

constexpr struct : xyz
{
    using xyz::xyz;
    using xyz::operator=;

    int a : 1;
    int b : 7;
} x {};

template<class T>
using is_pointer = std::is_pointer<T>::type;

template<class T>
constexpr auto is_pointer_v = std::is_pointer<T>::value;

uint64_t namespaces()
{
    std::vector<T>;
    boost::vector<T>;
    detail::vector<T>;
    details::vector<T>;
    aux::vector<T>;
    internals::vector<T>;
    other::vector<T>;
}

#if 1
    double foo(const A);
#else
    double foo(const A);
#endif

#if 0
    double foo(const A);
#else
    double foo(const A);
#endif

int bar(void*p, void * pp)
{
# if 0
    double foo();
# else
    double foo();
# endif
}

static uint64_t intWithSuffix = 42ull;
static long intWithSuffixAndPrefix = 0b0101L;
static int octNum = 07232;
static int invalidOctNum = 09231;
static uint64_t hexNum = 0xDEADBEEF42;
static uint64_t invalidHexNum = 0xGLDFKG;
static char binNum = 0b0101010;

static int64_t intWithSuffix = -42LL;
static long intWithSuffixAndPrefix = -0b0101L; // BUG
static int octNum = -07232;
static int invalidOctNum = -09231;
static int64_t hexNum = -0xDEADBEEF42;
static int64_t invalidHexNum = -0xGLDFKG;
static char binNum = -0b0101010;

static uint64_t intWithSuffixWithSep = 4'2ull;
static long intWithSuffixAndPrefixWithSep = 0b0'10'1L;
static int octNumWithSep = 07'232;
static int invalidOctNumWithSep = 09'23'1;
static uint64_t hexNumWithSep = 0xD'EAD'BE'EF'42;
static uint64_t invalidHexNumWithSep = 0xGLD'FKG;
static char binNumWithSep = 0b0'1010'10;

static double d1 = 42.;
static double d2 = .42;
static double d3 = 42.3e1;
static double d4 = .2e-12;
static double d5 = 32.e+12;
static double invalidD1 = 32.e+a12;
static float floatQualifier = 23.123f;
// TODO c++17
static double d6 = 0x1ffp10;
static double d7 = 0X0p-1;
static double d8 = 0x1.p0;
static double d9 = 0xf.p-1;
static double d10 = 0x0.123p-1;
static double d11 = 0xa.bp10l;
static double invalidD2 = 0x0.123p-a;
static float floatQualifier = 0xf.p-1f;

static bool yes = true;
static bool no = false;


static const char c1 = 'c';
static const char       c1a = u8'c'; // TODO c++17
static const char16_t   c1b = u'c';
static const char32_t   c1c = U'c';
static const wchar_t    c1d = L'c';
static const char c2 = '\n';
static const char c2a = '\120'; // octal
static const char c2b = '\x1f'; // hex
static const char c2c = '\'';
static const char c2d = '\\';
static const char* c3 = "string";
static const char* c4 = "\"string\n\t\012\x12\"";
static const char* c5 = "multiline \
    string";
static const char* c6 = "multifragment" "other""string";
static const char*      c6a = u8"string";
static const char16_t*  c6b = u"string";
static const char32_t*  c6c = U"string";
static const wchar_t*   c6d = L"string";
static const char* rawString1 = R"(string)";
static const char*      rawString1a = u8R"(string)";
static const char16_t*  rawString1b = uR"(string)";
static const char32_t*  rawString1c = UR"(string)";
static const wchar_t*   rawString1d = LR"(string\nstring)";
static const char* rawString2 = R"ab(string\nstring)ab";
static const char* rawString3 = R"ab(string
string)ab";

// UDL (c++11)

operator""_a(const char*); // BUG operator not colorized
operator ""_a(const char*); // BUG colorized or
operator "" _a(const char*); // BUG or not colorized
operator "" a(const char*);
operator ""a(const char*); // BUG
operator""a(const char*); // BUG

"string"_s; // BUG colorized or
"string"s; // BUG or not colorized
"string"_s-b; // -b is not part of the string

// Macro

MY_XXX;
BOOST_XXX;
__STDC_VERSION__;
__TIME__;
__cplusplus;

// Attributes

[[noreturn]] void foo();
[[deprecated]] void foo();
[[deprecated("because")]] void foo();
void foo([[carries_dependency]] int);

[[opt(1), debug]]
[[using CC: opt(1), debug]] // c++17
[[using CC: CC::opt(1)]] // c++17

void f([[maybe_unused]] int val)
{
    [[maybe_unused]] int x;
    switch (x = foo(); x) {
        case 1:
            [[fallthrough]];
        case XXX:
        case Class::foo():
            [[fallthrough]];
        default:
            ;
    }

    // c++17: fold expression
    (args + ... + (1 * 2));
    (v.push_back(args), ...);

    [[omp::parallel]] for (auto&& x : v)
        x;
    for (auto&& [first,second] : mymap) {
    }

    auto [x, y] = foo();

    [x = 1, =y](){};

    decltype((auto)) x = foo();
}

auto f() -> decltype(foo());

__attribute__((pure)) void f();

label:
goto label;


//BEGIN region
// TODO comment FIXME comment ### comment BUG comment
//END region

#wrong

// \brief blah blah
/// \brief blah blah

/**
 * Doxygen
 * @param p1 text
 * \brief <b>bold text</b>
 * \dot
 * a -> b
 * \enddot
 *
 * \verbatim
 * <dummy>
 * \endverbatim
 * <html>text</html>
 */

#endif
