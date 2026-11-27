/*
    SPDX-FileCopyrightText: 2026 Reda Hilal <reda0x04@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "test-config.h"

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Format>
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
    Definition m_markdown;
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

    static Format formatByName(const Definition &definition, const QString &name)
    {
        for (const auto &format : definition.formats()) {
            if (format.name() == name) {
                return format;
            }
        }
        return Format();
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
        m_markdown = m_repo.definitionForName(QStringLiteral("Markdown"));
        QVERIFY(m_markdown.isValid());
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

    void testMarkdownHyperlinkFormats()
    {
        const QStringList linkFormats{
            QStringLiteral("Normal Text: Link"),
            QStringLiteral("Blockquote: Link"),
            QStringLiteral("List: Link"),
            QStringLiteral("Link"),
            QStringLiteral("Reference-Link: Link"),
            QStringLiteral("Reference-Link Target: Link"),
            QStringLiteral("Inline Image: Link"),
        };
        for (const auto &name : linkFormats) {
            const auto format = formatByName(m_markdown, name);
            QVERIFY2(format.isValid(), qPrintable(name));
            QVERIFY2(format.isHyperlink(), qPrintable(name));
        }

        // Underlining on its own does not make something a hyperlink, e.g.
        // the label of [label](https://example.com) is not a URL.
        const QStringList textFormats{
            QStringLiteral("Normal Text"),
            QStringLiteral("Emphasis Text"),
            QStringLiteral("Reference-Link Name"),
            QStringLiteral("Auto-Link"),
        };
        for (const auto &name : textFormats) {
            const auto format = formatByName(m_markdown, name);
            QVERIFY2(format.isValid(), qPrintable(name));
            QVERIFY2(!format.isHyperlink(), qPrintable(name));
        }
    }

    void testOtherDefinitions()
    {
        const auto rest = m_repo.definitionForName(QStringLiteral("reStructuredText"));
        QVERIFY(rest.isValid());
        QVERIFY(formatByName(rest, QStringLiteral("StandaloneHyperlink")).isHyperlink());
        QVERIFY(!formatByName(rest, QStringLiteral("HyperlinkReference")).isHyperlink());

        const auto org = m_repo.definitionForName(QStringLiteral("Org Mode"));
        QVERIFY(org.isValid());
        QVERIFY(formatByName(org, QStringLiteral("Normal Text: Link")).isHyperlink());
    }

    void testHyperlinkAnchors()
    {
        QTextDocument doc;
        SyntaxHighlighter highlighter(&doc);
        highlighter.setTheme(m_theme);
        highlighter.setDefinition(m_markdown);

        const QString text = QStringLiteral(
            "See https://example.com/a and <https://kde.org> and [label](https://kate-editor.org).\n"
            "More: https://example.org/b\n"
            "[foo]: https://example.net/\n");
        doc.setPlainText(text);
        highlighter.rehighlight();

        auto anchorHrefAt = [&doc](int position) {
            const auto block = doc.findBlock(position);
            const int positionInBlock = position - block.position();
            for (const auto &range : block.layout()->formats()) {
                if (positionInBlock >= range.start && positionInBlock < range.start + range.length && range.format.isAnchor()) {
                    return range.format.anchorHref();
                }
            }
            return QString();
        };

        auto checkLink = [&anchorHrefAt, &text, &doc](const QString &url) {
            const int pos = text.indexOf(url);
            QVERIFY(pos >= 0);
            QCOMPARE(anchorHrefAt(pos), url);
            // also in the middle of the link.
            QCOMPARE(anchorHrefAt(pos + 3), url);
        };

        checkLink(QStringLiteral("https://example.com/a"));
        checkLink(QStringLiteral("https://kde.org"));
        checkLink(QStringLiteral("https://kate-editor.org"));
        checkLink(QStringLiteral("https://example.org/b"));
        checkLink(QStringLiteral("https://example.net/"));

        // The label of [label](https://kate-editor.org) is not itself a URL.
        const int labelPos = text.indexOf(QStringLiteral("label"));
        QCOMPARE_GE(labelPos, 0);
        QVERIFY(anchorHrefAt(labelPos).isEmpty());
    }
};

QTEST_MAIN(SyntaxHighlighterTest)

#include "syntaxhighlighter_test.moc"
