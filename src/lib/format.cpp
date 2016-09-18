/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "format.h"
#include "theme.h"
#include "xml_p.h"

#include <QColor>
#include <QDebug>
#include <QMetaEnum>
#include <QXmlStreamReader>

using namespace SyntaxHighlighting;

namespace SyntaxHighlighting {
class FormatPrivate
{
public:
    FormatPrivate();
    QString name;
    QColor color;
    QColor selColor;
    QColor backgroundColor;
    QColor selBackgroundColor;
    Theme::TextStyle defaultStyle;
    bool italic;
    bool bold;
    bool underline;
    bool strikeout;
    bool spellCheck;

    // flags indicating wheter the itemData contains this info
    bool hasItalic;
    bool hasBold;
    bool hasUnderline;
    bool hasStrikeout;
    bool hasColor;
    bool hasSelColor;
    bool hasBgColor;
    bool hasSelBgColor;
};
}

static Theme::TextStyle stringToDefaultFormat(const QStringRef &str)
{
    if (!str.startsWith(QLatin1String("ds")))
        return Theme::Normal;

    static const auto idx = Theme::staticMetaObject.indexOfEnumerator("TextStyle");
    Q_ASSERT(idx >= 0);
    const auto metaEnum = Theme::staticMetaObject.enumerator(idx);

    bool ok = false;
    const auto value = metaEnum.keyToValue(str.mid(2).toLatin1().constData(), &ok);
    if (!ok || value < 0)
        return Theme::Normal;
    return static_cast<Theme::TextStyle>(value);
}

FormatPrivate::FormatPrivate()
    : defaultStyle(Theme::Normal)
    , italic(false)
    , bold(false)
    , underline(false)
    , strikeout(false)
    , spellCheck(true)
    , hasItalic(false)
    , hasBold(false)
    , hasUnderline(false)
    , hasStrikeout(false)
    , hasColor(false)
    , hasSelColor(false)
    , hasBgColor(false)
    , hasSelBgColor(false)
{
}

Format::Format() : d(new FormatPrivate)
{
}

Format::~Format()
{
}

bool Format::isValid() const
{
    return !d->name.isEmpty();
}

QString Format::name() const
{
    return d->name;
}

bool Format::isNormal(const Theme &theme) const
{
    return !hasTextColor(theme) && !d->hasSelColor && !hasBackgroundColor(theme)
        && !isBold(theme) && !isItalic(theme) && !isUnderline(theme) && !isStrikeThrough(theme);
}

bool Format::hasTextColor(const Theme &theme) const
{
    return d->hasColor || (theme.textColor(d->defaultStyle) & 0xff000000);
}

QColor Format::textColor(const Theme &theme) const
{
    return d->hasColor ? d->color : QColor(theme.textColor(d->defaultStyle));
}

QColor Format::selectedTextColor(const Theme &theme) const
{
    return d->hasSelColor ? d->selColor : QColor(theme.selectedTextColor(d->defaultStyle));
}

bool Format::hasBackgroundColor(const Theme &theme) const
{
    return d->hasBgColor || (theme.backgroundColor(d->defaultStyle) & 0xff000000);
}

QColor Format::backgroundColor(const Theme &theme) const
{
    return d->hasBgColor ? d->backgroundColor
                         : QColor(theme.backgroundColor(d->defaultStyle));
}

QColor Format::selectedBackgroundColor(const Theme &theme) const
{
    return d->hasSelBgColor ? d->selBackgroundColor
                            : QColor(theme.selectedBackgroundColor(d->defaultStyle));
}

bool Format::isBold(const Theme &theme) const
{
    return d->hasBold ? d->bold : theme.isBold(d->defaultStyle);
}

bool Format::isItalic(const Theme &theme) const
{
    return d->hasItalic ? d->italic : theme.isItalic(d->defaultStyle);
}

bool Format::isUnderline(const Theme &theme) const
{
    return d->hasUnderline ? d->underline : theme.isUnderline(d->defaultStyle);
}

bool Format::isStrikeThrough(const Theme &theme) const
{
    return d->hasStrikeout ? d->strikeout : theme.isStrikeThrough(d->defaultStyle);
}

bool Format::spellCheck() const
{
    return d->spellCheck;
}

void Format::load(QXmlStreamReader& reader)
{
    d->name = reader.attributes().value(QStringLiteral("name")).toString();
    d->defaultStyle = stringToDefaultFormat(reader.attributes().value(QStringLiteral("defStyleNum")));

    QStringRef ref = reader.attributes().value(QStringLiteral("color"));
    if (!ref.isEmpty()) {
        d->hasColor = true;
        d->color = QColor(ref.toString());
    }

    ref = reader.attributes().value(QStringLiteral("selColor"));
    if (!ref.isEmpty()) {
        d->hasSelColor = true;
        d->selColor = QColor(ref.toString());
    }

    ref = reader.attributes().value(QStringLiteral("backgroundColor"));
    if (!ref.isEmpty()) {
        d->hasBgColor = true;
        d->backgroundColor = QColor(ref.toString());
    }

    ref = reader.attributes().value(QStringLiteral("selBackgroundColor"));
    if (!ref.isEmpty()) {
        d->hasSelBgColor = true;
        d->selBackgroundColor = QColor(ref.toString());
    }

    ref = reader.attributes().value(QStringLiteral("italic"));
    if (!ref.isEmpty()) {
        d->hasItalic = true;
        d->italic = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("bold"));
    if (!ref.isEmpty()) {
        d->hasBold = true;
        d->bold = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("underline"));
    if (!ref.isEmpty()) {
        d->hasUnderline = true;
        d->underline = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("strikeOut"));
    if (!ref.isEmpty()) {
        d->hasStrikeout = true;
        d->strikeout = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("spellChecking"));
    if (!ref.isEmpty()) {
        d->spellCheck = Xml::attrToBool(ref);
    }
}
