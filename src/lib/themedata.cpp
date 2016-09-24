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
#include "syntaxhighlighting_logging.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMetaEnum>

#include <QDebug>

using namespace SyntaxHighlighting;

ThemeData* ThemeData::get(const Theme &theme)
{
    return theme.m_data.get();
}

ThemeData::ThemeData()
    : m_revision(0)
    , m_readOnly(true)
    , m_areaBackgroundColor(0)
    , m_codeFoldingColor(0)
    , m_brackedMatchingColor(0)
    , m_currentLineColor(0)
    , m_iconBorderColor(0)
    , m_indentationLineColor(0)
    , m_lineNumberColor(0)
    , m_currentLineNumberColor(0)
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

static inline TextStyleData readThemeData(const QJsonObject &obj)
{
    TextStyleData td;

    td.textColor = readColor(obj.value(QLatin1String("text-color")));
    td.backgroundColor = readColor(obj.value(QLatin1String("background-color")));
    td.selectedTextColor = readColor(obj.value(QLatin1String("selected-text-color")));
    td.selectedBackgroundColor = readColor(obj.value(QLatin1String("selected-background-color")));

    auto val = obj.value(QLatin1String("bold"));
    if (val.isBool()) {
        td.bold = val.toBool();
        td.hasBold = true;
    }
    val = obj.value(QLatin1String("italic"));
    if (val.isBool()) {
        td.italic = val.toBool();
        td.hasItalic = true;
    }
    val = obj.value(QLatin1String("underline"));
    if (val.isBool()) {
        td.underline = val.toBool();
        td.hasUnderline = true;
    }
    val = obj.value(QLatin1String("strike-through"));
    if (val.isBool()) {
        td.strikeThrough = val.toBool();
        td.hasStrikeThrough = true;
    }

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
        qCWarning(Log) << "Failed to parse theme file" << filePath << ":" << parseError.errorString();
        return false;
    }

    m_filePath = filePath;

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

    // read editor area colors
    const QJsonObject editorColors = obj.value(QLatin1String("editor-colors")).toObject();
    m_areaBackgroundColor = readColor(editorColors.value(QLatin1String("background-color")));
    m_codeFoldingColor = readColor(editorColors.value(QLatin1String("code-folding")));
    m_brackedMatchingColor = readColor(editorColors.value(QLatin1String("bracket-matching")));
    m_currentLineColor = readColor(editorColors.value(QLatin1String("current-line")));
    m_iconBorderColor = readColor(editorColors.value(QLatin1String("icon-border")));
    m_indentationLineColor = readColor(editorColors.value(QLatin1String("indentation-line")));
    m_lineNumberColor = readColor(editorColors.value(QLatin1String("line-numbers")));
    m_currentLineNumberColor = readColor(editorColors.value(QLatin1String("current-line-number")));

    // read per-definition style overrides
    const auto customStyles = obj.value(QLatin1String("custom-styles")).toObject();
    for (auto it = customStyles.begin(); it != customStyles.end(); ++it) {
        const auto obj = it.value().toObject();
        QHash<QString, TextStyleData> overrideStyle;
        for (auto it2 = obj.begin(); it2 != obj.end(); ++it2)
            overrideStyle.insert(it2.key(), readThemeData(it2.value().toObject()));
        m_textStyleOverrides.insert(it.key(), overrideStyle);
    }

    return true;
}

QString ThemeData::name() const
{
    return m_name;
}

int ThemeData::revision() const
{
    return m_revision;
}

bool ThemeData::isReadOnly() const
{
    return m_readOnly;
}

QString ThemeData::filePath() const
{
    return m_filePath;
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

QRgb ThemeData::areaBackgroundColor() const
{
    return m_areaBackgroundColor;
}

QRgb ThemeData::codeFoldingColor() const
{
    return m_codeFoldingColor;
}

QRgb ThemeData::brackedMatchingColor() const
{
    return m_brackedMatchingColor;
}

QRgb ThemeData::currentLineColor() const
{
    return m_currentLineColor;
}

QRgb ThemeData::iconBorderColor() const
{
    return m_iconBorderColor;
}

QRgb ThemeData::indentationLineColor() const
{
    return m_indentationLineColor;
}

QRgb ThemeData::lineNumberColor() const
{
    return m_lineNumberColor;
}

QRgb ThemeData::currentLineNumberColor() const
{
    return m_currentLineNumberColor;
}

TextStyleData ThemeData::textStyleOverride(const QString& definitionName, const QString& attributeName) const
{
    return m_textStyleOverrides.value(definitionName).value(attributeName);
}
