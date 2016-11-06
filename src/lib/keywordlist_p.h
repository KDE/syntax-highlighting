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

#ifndef KSYNTAXHIGHLIGHTING_KEYWORDLIST_P_H
#define KSYNTAXHIGHLIGHTING_KEYWORDLIST_P_H

#include <QSet>
#include <QString>
#include <QVector>

class QXmlStreamReader;

namespace KSyntaxHighlighting {

class KeywordList
{
public:
    KeywordList();
    ~KeywordList();

    bool isEmpty() const;

    QString name() const;

    /** Checks if @p str is a keyword in this list. */
    bool contains(const QStringRef &str) const;
    /** Checks if @p str is a keyword in this list, overriding the global case-sensitivity setting. */
    bool contains(const QStringRef &str, Qt::CaseSensitivity caseSensitivityOverride) const;

    void load(QXmlStreamReader &reader);
    void setCaseSensitivity(Qt::CaseSensitivity caseSensitive);

private:
    QString m_name;
    QSet<QString> m_keywords;
    mutable QSet<QString> m_lowerCaseKeywords;
    Qt::CaseSensitivity m_caseSensitive;
};
}

#endif // KSYNTAXHIGHLIGHTING_KEYWORDLIST_P_H
