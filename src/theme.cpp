#include "theme.h"

using namespace KateSyntax;

namespace KateSyntax {

struct ThemeData {
    QRgb normalColor;
    QRgb backgroundColor;
    bool bold;
    bool underline;
};

static const ThemeData normal_default_theme[] {
    { 0x00000000, 0x00000000, false, false }, // Normal
    { 0xff1f1c1b, 0x00000000, true , false }, // Keyword
    { 0xff644a9b, 0x00000000, false, false }, // Function
    { 0xff0057ae, 0x00000000, false, false }, // Variable
    { 0xff1f1c1b, 0x00000000, true , false }, // ControlFlow
    { 0xff1f1c1b, 0x00000000, false, false }, // Operator
    { 0xff644a9b, 0x00000000, false, false }, // BuiltIn
    { 0xff0095ff, 0x00000000, true , false }, // Extension
    { 0xff006e28, 0x00000000, false, false }, // Preprocessor
    { 0xff0057ae, 0x00000000, false, false }, // Attribute
    { 0xff924c9d, 0x00000000, false, false }, // Char
    { 0xff3daee9, 0x00000000, false, false }, // SpecialChar
    { 0xffbf0303, 0x00000000, false, false }, // String
    { 0xff0057ae, 0x00000000, false, false }, // VerbatimString
    { 0xffff5500, 0x00000000, false, false }, // SpecialString
    { 0xffff5500, 0x00000000, false, false }, // Import
    { 0xff0057ae, 0x00000000, false, false }, // DataType
    { 0xffb08000, 0x00000000, false, false }, // DecVal
    { 0xffb08000, 0x00000000, false, false }, // BaseN
    { 0xffb08000, 0x00000000, false, false }, // Float
    { 0xffaa5500, 0x00000000, false, false }, // Constant
    { 0xff898887, 0x00000000, false, false }, // Comment
    { 0xff607880, 0x00000000, false, false }, // Documentation
    { 0xffca60ca, 0x00000000, false, false }, // Annotation
    { 0xff0095ff, 0x00000000, false, false }, // CommentVar
    { 0xff0057ae, 0xffe0e9f8, false, false }, // RegionMarker
    { 0xffb08000, 0x00000000, false, false }, // Information
    { 0xffbf0303, 0x00000000, false, false }, // Warning
    { 0xffbf0303, 0xfff7e6e6, true , false }, // Alert
    { 0xffbf0303, 0x00000000, false, true  }, // Error
    { 0xff006e28, 0x00000000, false, false }, // Others
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

bool Theme::isBold(KateSyntax::Theme::Style style) const
{
    Q_ASSERT(m_data);
    return m_data.get()[style].bold;
}

bool Theme::isUnderline(Theme::Style style) const
{
    Q_ASSERT(m_data);
    return m_data.get()[style].underline;
}
