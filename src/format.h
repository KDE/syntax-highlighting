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

#ifndef KATESYNTAX_FORMAT_H
#define KATESYNTAX_FORMAT_H

#include "katesyntax_export.h"

#include <QColor>
#include <QString>

class QXmlStreamReader;

namespace KateSyntax {

class KATESYNTAX_EXPORT Format
{
public:
    Format();
    ~Format();

    QString name() const;
    QColor color() const;

    void load(QXmlStreamReader &reader);

private:
    QString m_name;
    QColor m_color;
    QColor m_selColor;
    bool m_italic;
    bool m_bold;
    bool m_underline;
    bool m_strikeout;
};
}

#endif // KATESYNTAX_FORMAT_H
