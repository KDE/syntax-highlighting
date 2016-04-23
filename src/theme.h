#ifndef KATESYNTAX_THEME_H
#define KATESYNTAX_THEME_H

#include <QColor>

#include <memory>

namespace KateSyntax {

struct ThemeData;

class Theme
{
public:
    ~Theme();

    enum Style {
        Normal,
        Keyword,
        Function,
        Variable,
        ControlFlow,
        Operator,
        BuiltIn,
        Extension,
        Preprocessor,
        Attribute,
        Char,
        SpecialChar,
        String,
        VerbatimString,
        SpecialString,
        Import,
        DataType,
        DecVal,
        BaseN,
        Float,
        Constant,
        Comment,
        Documentation,
        Annotation,
        CommentVar,
        RegionMarker,
        Information,
        Warning,
        Alert,
        Error,
        Others
    };

    QRgb normalColor(Style style) const;
    QRgb backgroundColor(Style style) const;

    bool isBold(Style style) const;
    bool isItalic(Style style) const;
    bool isUnderline(Style style) const;
    bool isStrikeThrough(Style style) const;

    enum DefaultTheme {
        NormalTheme,
        DarkTheme
    };
    static Theme defaultTheme(DefaultTheme t = NormalTheme);

private:
    Theme();
    std::shared_ptr<ThemeData> m_data;
};

}

#endif // KATESYNTAX_THEME_H
