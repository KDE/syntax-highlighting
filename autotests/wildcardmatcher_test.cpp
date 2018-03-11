/* This file is part of the KDE libraries
   Copyright (C) 2007 Sebastian Pipping <webmaster@hartwork.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include <wildcardmatcher_p.h>

#include <QObject>
#include <qtest.h>

class WildcardMatcherTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testPositiveMatch_data()
    {
        QTest::addColumn<QString>("str");
        QTest::addColumn<QString>("pattern");

        QTest::newRow("*.txt") << "abc.txt" << "*.txt";

        QTest::newRow("*Makefile* 1") << "Makefile.am" << "*Makefile*";
        QTest::newRow("*Makefile* 2") << "Makefile.am" << "Makefile*";

        QTest::newRow("control") << "control" << "control";

        QTest::newRow("a??d") << "abcd" << "a??d";

        QTest::newRow("?") << "a" << "?";
        QTest::newRow("*?*") << "a" << "*?*";
        QTest::newRow("*") << "a" << "*";
        QTest::newRow("**") << "a" << "**";
        QTest::newRow("***") << "a" << "***";

        QTest::newRow("empty 1") << "" << "*";
        QTest::newRow("empty 2") << "" << "**";

        QTest::newRow("a*") << "ab" << "a*";
        QTest::newRow("*b") << "ab" << "*b";
        QTest::newRow("a?") << "ab" << "a?";
        QTest::newRow("?b") << "ab" << "?b";

        QTest::newRow("a*b*c") << "aXXbXXbYYaYc" << "a*b*c";
    }

    void testPositiveMatch()
    {
        QFETCH(QString, str);
        QFETCH(QString, pattern);
        QVERIFY(KSyntaxHighlighting::WildcardMatcher::exactMatch(str, pattern));
    }

    void testNegativeMatch_data()
    {
        QTest::addColumn<QString>("str");
        QTest::addColumn<QString>("pattern");

        QTest::newRow("*.cpp") << "abc.txt" << "*.cpp";
        QTest::newRow("*Makefile* 3") << "Makefile.am" << "Makefile";
        QTest::newRow("?") << "" << "?";
    }

    void testNegativeMatch()
    {
        QFETCH(QString, str);
        QFETCH(QString, pattern);
        QVERIFY(!KSyntaxHighlighting::WildcardMatcher::exactMatch(str, pattern));
    }
};

QTEST_GUILESS_MAIN(WildcardMatcherTest)

#include "wildcardmatcher_test.moc"
