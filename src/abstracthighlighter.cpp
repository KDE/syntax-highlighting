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

#include "abstracthighlighter.h"
#include "syntaxdefinition.h"

#include <QDebug>

using namespace KateSyntax;

AbstractHighlighter::AbstractHighlighter() :
    m_definition(nullptr)
{
}

AbstractHighlighter::~AbstractHighlighter()
{
}

void AbstractHighlighter::setDefinition(SyntaxDefinition* def)
{
    m_definition = def;
    m_context.clear();
    if (!m_definition)
        return;
    m_context.push(m_definition->initialContext());
}

void AbstractHighlighter::highlightLine(const QString& text)
{
    qDebug() << text;
    if (!m_definition) {
        setFormat(0, text.size(), QStringLiteral("dsNormal"));
        return;
    }

    setFormat(0, text.size(), QStringLiteral("dsNormal"));
}

void AbstractHighlighter::setFormat(int offset, int length, const QString& format)
{
    qDebug() << offset << length << format;
}
