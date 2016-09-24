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
#include "textstyledata_p.h"
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
    TextStyleData style;
    Theme::TextStyle defaultStyle;
    bool spellCheck;
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
    , spellCheck(true)
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

bool Format::isDefaultTextStyle(const Theme &theme) const
{
    return (!hasTextColor(theme))
        && (!hasBackgroundColor(theme))
        && (selectedTextColor(theme) == theme.selectedTextColor(Theme::Normal))
        && (selectedBackgroundColor(theme) == theme.selectedBackgroundColor(Theme::Normal))
        && (isBold(theme) == theme.isBold(Theme::Normal))
        && (isItalic(theme) == theme.isItalic(Theme::Normal))
        && (isUnderline(theme) == theme.isUnderline(Theme::Normal))
        && (isStrikeThrough(theme) == theme.isStrikeThrough(Theme::Normal));
}

bool Format::hasTextColor(const Theme &theme) const
{
    return textColor(theme) != theme.textColor(Theme::Normal)
        && (d->style.textColor || theme.textColor(d->defaultStyle));
}

QColor Format::textColor(const Theme &theme) const
{
    return d->style.textColor ? d->style.textColor : theme.textColor(d->defaultStyle);
}

QColor Format::selectedTextColor(const Theme &theme) const
{
    return d->style.selectedTextColor ? d->style.selectedTextColor : theme.selectedTextColor(d->defaultStyle);
}

bool Format::hasBackgroundColor(const Theme &theme) const
{
    return backgroundColor(theme) != theme.backgroundColor(Theme::Normal)
         && (d->style.backgroundColor || theme.backgroundColor(d->defaultStyle));
}

QColor Format::backgroundColor(const Theme &theme) const
{
    return d->style.backgroundColor ? d->style.backgroundColor : theme.backgroundColor(d->defaultStyle);
}

QColor Format::selectedBackgroundColor(const Theme &theme) const
{
    return d->style.selectedBackgroundColor ? d->style.selectedBackgroundColor
                            : theme.selectedBackgroundColor(d->defaultStyle);
}

bool Format::isBold(const Theme &theme) const
{
    return d->style.hasBold ? d->style.bold : theme.isBold(d->defaultStyle);
}

bool Format::isItalic(const Theme &theme) const
{
    return d->style.hasItalic ? d->style.italic : theme.isItalic(d->defaultStyle);
}

bool Format::isUnderline(const Theme &theme) const
{
    return d->style.hasUnderline ? d->style.underline : theme.isUnderline(d->defaultStyle);
}

bool Format::isStrikeThrough(const Theme &theme) const
{
    return d->style.hasStrikeThrough ? d->style.strikeThrough : theme.isStrikeThrough(d->defaultStyle);
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
        d->style.textColor = QColor(ref.toString()).rgba();
    }

    ref = reader.attributes().value(QStringLiteral("selColor"));
    if (!ref.isEmpty()) {
        d->style.selectedTextColor = QColor(ref.toString()).rgba();
    }

    ref = reader.attributes().value(QStringLiteral("backgroundColor"));
    if (!ref.isEmpty()) {
        d->style.backgroundColor = QColor(ref.toString()).rgba();
    }

    ref = reader.attributes().value(QStringLiteral("selBackgroundColor"));
    if (!ref.isEmpty()) {
        d->style.selectedBackgroundColor = QColor(ref.toString()).rgba();
    }

    ref = reader.attributes().value(QStringLiteral("italic"));
    if (!ref.isEmpty()) {
        d->style.hasItalic = true;
        d->style.italic = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("bold"));
    if (!ref.isEmpty()) {
        d->style.hasBold = true;
        d->style.bold = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("underline"));
    if (!ref.isEmpty()) {
        d->style.hasUnderline = true;
        d->style.underline = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("strikeOut"));
    if (!ref.isEmpty()) {
        d->style.hasStrikeThrough = true;
        d->style.strikeThrough = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("spellChecking"));
    if (!ref.isEmpty()) {
        d->spellCheck = Xml::attrToBool(ref);
    }
}
