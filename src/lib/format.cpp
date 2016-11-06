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
#include "format_p.h"
#include "definition.h"
#include "definitionref_p.h"
#include "textstyledata_p.h"
#include "theme.h"
#include "themedata_p.h"
#include "xml_p.h"

#include <QColor>
#include <QDebug>
#include <QMetaEnum>
#include <QXmlStreamReader>

using namespace KSyntaxHighlighting;

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
    , id(0)
    , spellCheck(true)
{
}

FormatPrivate* FormatPrivate::get(const Format &format)
{
    return format.d.data();
}

TextStyleData FormatPrivate::styleOverride(const Theme &theme) const
{
    const auto themeData = ThemeData::get(theme);
    if (themeData)
        return themeData->textStyleOverride(definition.definition().name(), name);
    return TextStyleData();
}

Format::Format() : d(new FormatPrivate)
{
}

Format::Format(const Format &other) :
    d(other.d)
{
}

Format::~Format()
{
}

Format& Format::operator=(const Format& other)
{
    d = other.d;
    return *this;
}

bool Format::isValid() const
{
    return !d->name.isEmpty();
}

QString Format::name() const
{
    return d->name;
}

quint16 Format::id() const
{
    return d->id;
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
    const auto overrideStyle = d->styleOverride(theme);
    return textColor(theme) != theme.textColor(Theme::Normal)
        && (d->style.textColor || theme.textColor(d->defaultStyle) || overrideStyle.textColor);
}

QColor Format::textColor(const Theme &theme) const
{
    const auto overrideStyle = d->styleOverride(theme);
    if (overrideStyle.textColor)
        return overrideStyle.textColor;
    return d->style.textColor ? d->style.textColor : theme.textColor(d->defaultStyle);
}

QColor Format::selectedTextColor(const Theme &theme) const
{
    const auto overrideStyle = d->styleOverride(theme);
    if (overrideStyle.selectedTextColor)
        return overrideStyle.selectedTextColor;
    return d->style.selectedTextColor ? d->style.selectedTextColor : theme.selectedTextColor(d->defaultStyle);
}

bool Format::hasBackgroundColor(const Theme &theme) const
{
    const auto overrideStyle = d->styleOverride(theme);
    return backgroundColor(theme) != theme.backgroundColor(Theme::Normal)
         && (d->style.backgroundColor || theme.backgroundColor(d->defaultStyle) || overrideStyle.backgroundColor);
}

QColor Format::backgroundColor(const Theme &theme) const
{
    const auto overrideStyle = d->styleOverride(theme);
    if (overrideStyle.backgroundColor)
        return overrideStyle.backgroundColor;
    return d->style.backgroundColor ? d->style.backgroundColor : theme.backgroundColor(d->defaultStyle);
}

QColor Format::selectedBackgroundColor(const Theme &theme) const
{
    const auto overrideStyle = d->styleOverride(theme);
    if (overrideStyle.selectedBackgroundColor)
        return overrideStyle.selectedBackgroundColor;
    return d->style.selectedBackgroundColor ? d->style.selectedBackgroundColor
                            : theme.selectedBackgroundColor(d->defaultStyle);
}

bool Format::isBold(const Theme &theme) const
{
    const auto overrideStyle = d->styleOverride(theme);
    if (overrideStyle.hasBold)
        return overrideStyle.bold;
    return d->style.hasBold ? d->style.bold : theme.isBold(d->defaultStyle);
}

bool Format::isItalic(const Theme &theme) const
{
    const auto overrideStyle = d->styleOverride(theme);
    if (overrideStyle.hasItalic)
        return overrideStyle.italic;
    return d->style.hasItalic ? d->style.italic : theme.isItalic(d->defaultStyle);
}

bool Format::isUnderline(const Theme &theme) const
{
    const auto overrideStyle = d->styleOverride(theme);
    if (overrideStyle.hasUnderline)
        return overrideStyle.underline;
    return d->style.hasUnderline ? d->style.underline : theme.isUnderline(d->defaultStyle);
}

bool Format::isStrikeThrough(const Theme &theme) const
{
    const auto overrideStyle = d->styleOverride(theme);
    if (overrideStyle.hasStrikeThrough)
        return overrideStyle.strikeThrough;
    return d->style.hasStrikeThrough ? d->style.strikeThrough : theme.isStrikeThrough(d->defaultStyle);
}

bool Format::spellCheck() const
{
    return d->spellCheck;
}


void FormatPrivate::load(QXmlStreamReader& reader)
{
    name = reader.attributes().value(QStringLiteral("name")).toString();
    defaultStyle = stringToDefaultFormat(reader.attributes().value(QStringLiteral("defStyleNum")));

    QStringRef ref = reader.attributes().value(QStringLiteral("color"));
    if (!ref.isEmpty()) {
        style.textColor = QColor(ref.toString()).rgba();
    }

    ref = reader.attributes().value(QStringLiteral("selColor"));
    if (!ref.isEmpty()) {
        style.selectedTextColor = QColor(ref.toString()).rgba();
    }

    ref = reader.attributes().value(QStringLiteral("backgroundColor"));
    if (!ref.isEmpty()) {
        style.backgroundColor = QColor(ref.toString()).rgba();
    }

    ref = reader.attributes().value(QStringLiteral("selBackgroundColor"));
    if (!ref.isEmpty()) {
        style.selectedBackgroundColor = QColor(ref.toString()).rgba();
    }

    ref = reader.attributes().value(QStringLiteral("italic"));
    if (!ref.isEmpty()) {
        style.hasItalic = true;
        style.italic = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("bold"));
    if (!ref.isEmpty()) {
        style.hasBold = true;
        style.bold = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("underline"));
    if (!ref.isEmpty()) {
        style.hasUnderline = true;
        style.underline = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("strikeOut"));
    if (!ref.isEmpty()) {
        style.hasStrikeThrough = true;
        style.strikeThrough = Xml::attrToBool(ref);
    }

    ref = reader.attributes().value(QStringLiteral("spellChecking"));
    if (!ref.isEmpty()) {
        spellCheck = Xml::attrToBool(ref);
    }
}
