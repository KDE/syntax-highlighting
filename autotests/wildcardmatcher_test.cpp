/*
    SPDX-FileCopyrightText: 2007 Sebastian Pipping <webmaster@hartwork.org>

    SPDX-License-Identifier: MIT
*/

#include <wildcardmatcher_p.h>

#include <QObject>
#include <QTest>

class WildcardMatcherTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testPositiveMatch_data()
    {
        QTest::addColumn<QString>("str");
        QTest::addColumn<QString>("pattern");

        QTest::newRow("*.txt") << "abc.txt"
                               << "*.txt";

        QTest::newRow("*Makefile* 1") << "Makefile.am"
                                      << "*Makefile*";
        QTest::newRow("*Makefile* 2") << "Makefile.am"
                                      << "Makefile*";

        QTest::newRow("control") << "control"
                                 << "control";

        QTest::newRow("a??d") << "abcd"
                              << "a??d";

        QTest::newRow("?") << "a"
                           << "?";
        QTest::newRow("*?*") << "a"
                             << "*?*";
        QTest::newRow("*") << "a"
                           << "*";
        QTest::newRow("**") << "a"
                            << "**";
        QTest::newRow("***") << "a"
                             << "***";

        QTest::newRow("empty 1") << ""
                                 << "*";
        QTest::newRow("empty 2") << ""
                                 << "**";

        QTest::newRow("a*") << "ab"
                            << "a*";
        QTest::newRow("*b") << "ab"
                            << "*b";
        QTest::newRow("a?") << "ab"
                            << "a?";
        QTest::newRow("?b") << "ab"
                            << "?b";

        QTest::newRow("a*b*c") << "aXXbXXbYYaYc"
                               << "a*b*c";
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

        QTest::newRow("*.cpp") << "abc.txt"
                               << "*.cpp";
        QTest::newRow("*Makefile* 3") << "Makefile.am"
                                      << "Makefile";
        QTest::newRow("?") << ""
                           << "?";
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
