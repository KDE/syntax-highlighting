/*
    SPDX-FileCopyrightText: 2026 Reda Hilal <reda0x04@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "test-config.h"

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Theme>

#include <QAbstractTextDocumentLayout>
#include <QCoreApplication>
#include <QObject>
#include <QStandardPaths>
#include <QTest>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>

using namespace KSyntaxHighlighting;

class SyntaxHighlighterTest : public QObject
{
    Q_OBJECT
private:
    Repository m_repo;
    Definition m_cpp;
    Theme m_theme;

    // without a layout, QTextDocument doesn't emit contentsChange() and nothing gets highlighted
    static void createLayout(QTextDocument &doc)
    {
        QVERIFY(doc.documentLayout());
    }

    static void runQueuedRehighlights(const QTextDocument &doc)
    {
        for (int i = 0; i <= doc.blockCount(); ++i) {
            QCoreApplication::sendPostedEvents();
        }
    }

    static QList<QList<QTextLayout::FormatRange>> blockFormats(const QTextDocument &doc)
    {
        QList<QList<QTextLayout::FormatRange>> formats;
        for (auto block = doc.begin(); block.isValid(); block = block.next()) {
            formats.push_back(block.layout()->formats());
        }
        return formats;
    }

    QList<QList<QTextLayout::FormatRange>> freshlyHighlightedFormats(const QString &text) const
    {
        QTextDocument doc;
        createLayout(doc);
        SyntaxHighlighter highlighter(&doc);
        highlighter.setTheme(m_theme);
        highlighter.setDefinition(m_cpp);
        doc.setPlainText(text);
        return blockFormats(doc);
    }

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
        initRepositorySearchPaths(m_repo);
        m_cpp = m_repo.definitionForName(QStringLiteral("C++"));
        QVERIFY(m_cpp.isValid());
        m_theme = m_repo.defaultTheme(Repository::LightTheme);
        QVERIFY(m_theme.isValid());
    }

    void testEditRehighlightsFollowingBlocks()
    {
        QTextDocument doc;
        createLayout(doc);
        SyntaxHighlighter highlighter(&doc);
        highlighter.setTheme(m_theme);
        highlighter.setDefinition(m_cpp);
        doc.setPlainText(QStringLiteral("int a;\nint b;\nint c;\n"));

        QTextCursor cursor(doc.firstBlock());
        cursor.insertText(QStringLiteral("/*"));
        runQueuedRehighlights(doc);

        QCOMPARE(blockFormats(doc), freshlyHighlightedFormats(doc.toPlainText()));
    }

    void testReplaceTextWhileRehighlightIsQueued()
    {
        // every line is in a comment, so switching to C++ queues a re-highlight for each one
        QString longText = QStringLiteral("/*\n");
        for (int i = 0; i < 20000; ++i) {
            longText += QStringLiteral("line %1\n").arg(i);
        }
        const QString shortText = QStringLiteral("int a;\nint b;\n");

        // no layout, it would make the test slow
        QTextDocument doc;
        SyntaxHighlighter highlighter(&doc);
        highlighter.setTheme(m_theme);
        doc.setPlainText(longText);
        highlighter.rehighlight();
        highlighter.setDefinition(m_cpp);

        doc.setPlainText(shortText);
        highlighter.rehighlight();
        runQueuedRehighlights(doc);

        QCOMPARE(blockFormats(doc), freshlyHighlightedFormats(shortText));
    }
};

QTEST_MAIN(SyntaxHighlighterTest)

#include "syntaxhighlighter_test.moc"
