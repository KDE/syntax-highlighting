#include "theme.h"

using namespace KateSyntax;

namespace KateSyntax {

struct ThemeData {
    QRgb normalColor;
    QRgb backgroundColor;
};

static const ThemeData normal_default_theme[] {
    { 0x00000000, 0x00000000 }, // Normal
    { 0xff1f1c1b, 0x00000000 }, // Keyword
    { 0xff644a9b, 0x00000000 }, // Function
    { 0xff0057ae, 0x00000000 }, // Variable
    { 0xff1f1c1b, 0x00000000 }, // ControlFlow
    { 0xff1f1c1b, 0x00000000 }, // Operator
    { 0xff644a9b, 0x00000000 }, // BuiltIn
    { 0xff0095ff, 0x00000000 }, // Extension
    { 0xff006e28, 0x00000000 }, // Preprocessor
    { 0xff0057ae, 0x00000000 }, // Attribute
    { 0xff924c9d, 0x00000000 }, // Char
    { 0xff3daee9, 0x00000000 }, // SpecialChar
    { 0xffbf0303, 0x00000000 }, // String
    { 0xff0057ae, 0x00000000 }, // VerbatimString
    { 0xffff5500, 0x00000000 }, // SpecialString
    { 0xffff5500, 0x00000000 }, // Import
    { 0xff0057ae, 0x00000000 }, // DataType
    { 0xffb08000, 0x00000000 }, // DecVal
    { 0xffb08000, 0x00000000 }, // BaseN
    { 0xffb08000, 0x00000000 }, // Float
    { 0xffaa5500, 0x00000000 }, // Constant
    { 0xff898887, 0x00000000 }, // Comment
    { 0xff607880, 0x00000000 }, // Documentation
    { 0xffca60ca, 0x00000000 }, // Annotation
    { 0xff0095ff, 0x00000000 }, // CommentVar
    { 0xff0057ae, 0xffe0e9f8 }, // RegionMarker
    { 0xffb08000, 0x00000000 }, // Information
    { 0xffbf0303, 0x00000000 }, // Warning
    { 0xffbf0303, 0xfff7e6e6 }, // Alert
    { 0xffbf0303, 0x00000000 }, // Error
    { 0xff006e28, 0x00000000 }, // Others
};

static void noDeleter(ThemeData*) {}

}

Theme::Theme()
{
}

Theme::~Theme()
{
}

Theme Theme::defaultTheme(Theme::DefaultTheme t)
{
    Theme theme;
    switch (t) {
        case NormalTheme:
            theme.m_data.reset(const_cast<ThemeData*>(normal_default_theme), noDeleter);
            break;
        case DarkTheme:
            // TODO
            break;
    }
    return theme;
}

QRgb Theme::normalColor(Theme::Style style) const
{
    Q_ASSERT(m_data);
    return m_data.get()[style].normalColor;
}

QRgb Theme::backgroundColor(Theme::Style style) const
{
    Q_ASSERT(m_data);
    return m_data.get()[style].backgroundColor;
}
