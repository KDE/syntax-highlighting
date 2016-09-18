/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>
    Copyright (C) 2016 Dominik Haumann <dhaumann@kde.org>

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

#include "themedata_p.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMetaEnum>

#include <QDebug>

using namespace SyntaxHighlighting;

ThemeData::ThemeData()
    : m_readOnly(true)
{
}

/**
 * Convert QJsonValue @p val into a color, if possible. Valid colors are only
 * in hex format: #rrggbb. On error, returns 0x00000000.
 */
static inline QRgb readColor(const QJsonValue &val)
{
    const QRgb unsetColor = 0;
    if (!val.isString()) {
        return unsetColor;
    }
    const QString str = val.toString();
    if (str.isEmpty() || str[0] != QLatin1Char('#')) {
        return unsetColor;
    }
    const QColor color(str);
    return color.isValid() ? color.rgb() : unsetColor;
}

/**
 * Convert QJsonValue @p val into a bool, if possible. On error, returns false.
 */
static inline bool readBool(const QJsonValue &val)
{
    return val.isBool() ? val.toBool() : false;
}

static inline TextStyleData readThemeData(const QJsonObject &obj)
{
    TextStyleData td;

    td.textColor = readColor(obj.value(QLatin1String("text-color")));
    td.backgroundColor = readColor(obj.value(QLatin1String("background-color")));
    td.selectedTextColor = readColor(obj.value(QLatin1String("selected-text-color")));
    td.selectedBackgroundColor = readColor(obj.value(QLatin1String("selected-background-color")));
    td.bold = readBool(obj.value(QLatin1String("bold")));
    td.italic = readBool(obj.value(QLatin1String("italic")));
    td.underline = readBool(obj.value(QLatin1String("underline")));
    td.strikeThrough = readBool(obj.value(QLatin1String("strike-through")));

    return td;
}

bool ThemeData::load(const QString &filePath)
{
    QFile loadFile(filePath);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    const QByteArray jsonData = loadFile.readAll();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return false;
    }

    QJsonObject obj = jsonDoc.object();

    // read metadata
    const QJsonObject metadata = obj.value(QLatin1String("metadata")).toObject();
    m_name = metadata.value(QLatin1String("name")).toString();
    m_revision = metadata.value(QLatin1String("revision")).toInt();
    m_author = metadata.value(QLatin1String("author")).toString();
    m_license = metadata.value(QLatin1String("license")).toString();
    m_readOnly = metadata.value(QLatin1String("read-only")).toBool();

    // read text styles
    static const auto idx = Theme::staticMetaObject.indexOfEnumerator("TextStyle");
    Q_ASSERT(idx >= 0);
    const auto metaEnum = Theme::staticMetaObject.enumerator(idx);
    const QJsonObject textStyles = obj.value(QLatin1String("text-styles")).toObject();
    for (int i = 0; i < metaEnum.keyCount(); ++i) {
        Q_ASSERT(i == metaEnum.value(i));
        m_textStyles[i] = readThemeData(textStyles.value(QLatin1String(metaEnum.key(i))).toObject());
    }

    return true;
}

QString ThemeData::name() const
{
    return m_name;
}

bool ThemeData::isReadOnly() const
{
    return m_readOnly;
}

QRgb ThemeData::textColor(Theme::TextStyle style) const
{
    Q_ASSERT(static_cast<int>(style) >= 0 && static_cast<int>(style) <= static_cast<int>(Theme::Others));
    return m_textStyles[style].textColor;
}

QRgb ThemeData::selectedTextColor(Theme::TextStyle style) const
{
    Q_ASSERT(static_cast<int>(style) >= 0 && static_cast<int>(style) <= static_cast<int>(Theme::Others));
    return m_textStyles[style].selectedTextColor;
}

QRgb ThemeData::backgroundColor(Theme::TextStyle style) const
{
    Q_ASSERT(static_cast<int>(style) >= 0 && static_cast<int>(style) <= static_cast<int>(Theme::Others));
    return m_textStyles[style].backgroundColor;
}

QRgb ThemeData::selectedBackgroundColor(Theme::TextStyle style) const
{
    Q_ASSERT(static_cast<int>(style) >= 0 && static_cast<int>(style) <= static_cast<int>(Theme::Others));
    return m_textStyles[style].selectedBackgroundColor;
}

bool ThemeData::isBold(Theme::TextStyle style) const
{
    Q_ASSERT(static_cast<int>(style) >= 0 && static_cast<int>(style) <= static_cast<int>(Theme::Others));
    return m_textStyles[style].bold;
}

bool ThemeData::isItalic(Theme::TextStyle style) const
{
    Q_ASSERT(static_cast<int>(style) >= 0 && static_cast<int>(style) <= static_cast<int>(Theme::Others));
    return m_textStyles[style].italic;
}

bool ThemeData::isUnderline(Theme::TextStyle style) const
{
    Q_ASSERT(static_cast<int>(style) >= 0 && static_cast<int>(style) <= static_cast<int>(Theme::Others));
    return m_textStyles[style].underline;
}

bool ThemeData::isStrikeThrough(Theme::TextStyle style) const
{
    Q_ASSERT(static_cast<int>(style) >= 0 && static_cast<int>(style) <= static_cast<int>(Theme::Others));
    return m_textStyles[style].strikeThrough;
}
