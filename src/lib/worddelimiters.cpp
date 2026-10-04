/*
    SPDX-FileCopyrightText: 2020 Jonathan Poelen <jonathan.poelen@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "worddelimiters_p.h"

#include <QHash> // qHashMulti

using namespace KSyntaxHighlighting;

WordDelimiters::WordDelimiters()
    : m_asciiDelimiters{}
{
    for (const char *p = "\t !%&()*+,-./:;<=>?[\\]^{|}~"; *p; ++p) {
        m_asciiDelimiters.set(*p);
    }
}

WordDelimiters::WordDelimiters(QStringView str)
    : m_asciiDelimiters{}
{
    append(str);
}

bool WordDelimiters::contains(QChar c) const
{
    if (c.unicode() < 128) {
        return m_asciiDelimiters.test(c.unicode());
    }
    // perf tells contains is MUCH faster than binary search here, very short array
    return m_notAsciiDelimiters.contains(c);
}

void WordDelimiters::append(QStringView s)
{
    for (QChar c : s) {
        if (c.unicode() < 128) {
            m_asciiDelimiters.set(c.unicode());
        } else {
            m_notAsciiDelimiters.append(c);
        }
    }
}

void WordDelimiters::remove(QStringView s)
{
    for (QChar c : s) {
        if (c.unicode() < 128) {
            m_asciiDelimiters.set(c.unicode(), false);
        } else {
            m_notAsciiDelimiters.remove(c);
        }
    }
}

size_t WordDelimiters::hash(size_t seed) const
{
    return qHashMulti(seed, m_notAsciiDelimiters, std::hash<decltype(m_asciiDelimiters)>()(m_asciiDelimiters));
}

bool WordDelimiters::operator==(const WordDelimiters &other) const = default;
