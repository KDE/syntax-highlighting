/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2018 Christoph Cullmann <cullmann@kde.org>
    SPDX-FileCopyrightText: 2020 Jonathan Poelen <jonathan.poelen+kde@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include <QChar>

namespace KSyntaxHighlighting
{

// Returns true when `c` is in the range '0' to '9'.
// Do not use QChar::isDigit() because match any digit in unicode (romain numeral, etc).
inline bool isDigit(QChar c)
{
    return (c <= QLatin1Char('9') && QLatin1Char('0') <= c);
}

}
