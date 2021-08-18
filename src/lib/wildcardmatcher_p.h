/*
    SPDX-FileCopyrightText: 2007 Sebastian Pipping <webmaster@hartwork.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KSYNTAXHIGHLIGHTING_WILDCARDMATCHER_P_H
#define KSYNTAXHIGHLIGHTING_WILDCARDMATCHER_P_H

#include <QStringView>

namespace KSyntaxHighlighting
{
namespace WildcardMatcher
{
/**
 * Matches a string against a given wildcard.
 * The wildcard supports '*' (".*" in regex) and '?' ("." in regex), not more.
 *
 * @param candidate       Text to match
 * @param wildcard        Wildcard to use
 * @param caseSensitive   Case-sensitivity flag
 * @return                True for an exact match, false otherwise
 */
bool exactMatch(QStringView candidate, QStringView wildcard, bool caseSensitive = true);
}

}

#endif // KSYNTAXHIGHLIGHTING_WILDCARDMATCHER_P_H
