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

#include <abstracthighlighter.h>
#include <definition.h>
#include <format.h>
#include <repository.h>
#include <state.h>
#include <theme.h>

#include <QFileInfo>
#include <QObject>
#include <QStandardPaths>
#include <qtest.h>

namespace KSyntaxHighlighting {

class FormatCollector : public AbstractHighlighter
{
public:
    using AbstractHighlighter::highlightLine;
    void applyFormat(int offset, int length, const Format &format) Q_DECL_OVERRIDE
    {
        Q_UNUSED(offset);
        Q_UNUSED(length);
        formatMap.insert(format.name(), format);
    }
    QHash<QString, Format> formatMap;
};

class ThemeTest : public QObject
{
    Q_OBJECT
private:
        Repository m_repo;

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::enableTestMode(true);
    }

    void testThemes()
    {
        QVERIFY(!m_repo.themes().isEmpty());
        Q_FOREACH (const auto &theme, m_repo.themes()) {
            QVERIFY(theme.isValid());
            QVERIFY(!theme.name().isEmpty());
            QVERIFY(!theme.filePath().isEmpty());
            QVERIFY(QFileInfo::exists(theme.filePath()));
            QVERIFY(m_repo.theme(theme.name()).isValid());
        }
    }

    void testFormat_data()
    {
        QTest::addColumn<QString>("themeName");
        QTest::newRow("default") << "Default";
        QTest::newRow("dark") << "Breeze Dark";
        QTest::newRow("print") << "Printing";
    }

    void testFormat()
    {
        QFETCH(QString, themeName);

        // somewhat complicated way to get proper Format objects
        FormatCollector collector;
        collector.setDefinition(m_repo.definitionForName(QLatin1String("QML")));
        const auto t = m_repo.theme(themeName);
        QVERIFY(t.isValid());
        collector.setTheme(t);
        collector.highlightLine(QLatin1String("normal + property real foo: 3.14"), State());

        QVERIFY(collector.formatMap.size() >= 4);
        qDebug() << collector.formatMap.keys();

        // normal text
        auto f = collector.formatMap.value(QLatin1String("Normal Text"));
        QVERIFY(f.isValid());
        QVERIFY(f.isDefaultTextStyle(t));
        QVERIFY(!f.hasTextColor(t));
        QVERIFY(!f.hasBackgroundColor(t));
        QVERIFY(f.id() > 0);

        // visually identical to normal text
        f = collector.formatMap.value(QLatin1String("Symbol"));
        QVERIFY(f.isValid());
        QVERIFY(f.isDefaultTextStyle(t));
        QVERIFY(!f.hasTextColor(t));
        QVERIFY(f.id() > 0);

        // visually different to normal text
        f = collector.formatMap.value(QLatin1String("Keywords"));
        QVERIFY(f.isValid());
        QVERIFY(!f.isDefaultTextStyle(t));
        QVERIFY(f.isBold(t));
        QVERIFY(f.id() > 0);

        f = collector.formatMap.value(QLatin1String("Float"));
        QVERIFY(f.isValid());
        QVERIFY(!f.isDefaultTextStyle(t));
        QVERIFY(f.hasTextColor(t));
        QVERIFY(f.id() > 0);
    }

    void testDefaultTheme()
    {
        Theme t = m_repo.theme(QLatin1String("Default"));
        QVERIFY(t.isValid());

        // Themes compiled in as resource are never writable
        QVERIFY(t.isReadOnly());

        // make sure all editor colors are properly read
        QCOMPARE(t.editorColor(Theme::BackgroundColor)  , QColor("#ffffff").rgb());
        QCOMPARE(t.editorColor(Theme::TextSelection)    , QColor("#94caef").rgb());
        QCOMPARE(t.editorColor(Theme::CurrentLine)      , QColor("#f8f7f6").rgb());
        QCOMPARE(t.editorColor(Theme::SearchHighlight)  , QColor("#ffff00").rgb());
        QCOMPARE(t.editorColor(Theme::ReplaceHighlight) , QColor("#00ff00").rgb());
        QCOMPARE(t.editorColor(Theme::BracketMatching)  , QColor("#ffff00").rgb());
        QCOMPARE(t.editorColor(Theme::TabMarker)        , QColor("#d2d2d2").rgb());
        QCOMPARE(t.editorColor(Theme::SpellChecking)    , QColor("#bf0303").rgb());
        QCOMPARE(t.editorColor(Theme::IndentationLine)  , QColor("#d2d2d2").rgb());
        QCOMPARE(t.editorColor(Theme::IconBorder)       , QColor("#f0f0f0").rgb());
        QCOMPARE(t.editorColor(Theme::CodeFolding)      , QColor("#94caef").rgb());
        QCOMPARE(t.editorColor(Theme::LineNumbers)      , QColor("#a0a0a0").rgb());
        QCOMPARE(t.editorColor(Theme::CurrentLineNumber), QColor("#1e1e1e").rgb());
        QCOMPARE(t.editorColor(Theme::WordWrapMarker)   , QColor("#ededed").rgb());
        QCOMPARE(t.editorColor(Theme::ModifiedLines)    , QColor("#fdbc4b").rgb());
        QCOMPARE(t.editorColor(Theme::SavedLines)       , QColor("#2ecc71").rgb());
        QCOMPARE(t.editorColor(Theme::Separator)        , QColor("#898887").rgb());
        QCOMPARE(t.editorColor(Theme::MarkBookmark)     , QColor("#0000ff").rgb());
        QCOMPARE(t.editorColor(Theme::MarkBreakpointActive), QColor("#ff0000").rgb());
        QCOMPARE(t.editorColor(Theme::MarkBreakpointReached), QColor("#ffff00").rgb());
        QCOMPARE(t.editorColor(Theme::MarkBreakpointDisabled), QColor("#ff00ff").rgb());
        QCOMPARE(t.editorColor(Theme::MarkExecution)    , QColor("#a0a0a4").rgb());
        QCOMPARE(t.editorColor(Theme::MarkWarning)      , QColor("#00ff00").rgb());
        QCOMPARE(t.editorColor(Theme::MarkError)        , QColor("#ff0000").rgb());
        QCOMPARE(t.editorColor(Theme::TemplateBackground), QColor("#d6d2d0").rgb());
        QCOMPARE(t.editorColor(Theme::TemplatePlaceholder), QColor("#baf8ce").rgb());
        QCOMPARE(t.editorColor(Theme::TemplateFocusedPlaceholder), QColor("#76da98").rgb());
        QCOMPARE(t.editorColor(Theme::TemplateReadOnlyPlaceholder), QColor("#f6e6e6").rgb());
    }

    void testInvalidTheme()
    {
        // somewhat complicated way to get proper Format objects
        FormatCollector collector;
        collector.setDefinition(m_repo.definitionForName(QLatin1String("QML")));
        collector.highlightLine(QLatin1String("normal + property real foo: 3.14"), State());

        QVERIFY(collector.formatMap.size() >= 4);
        auto f = collector.formatMap.value(QLatin1String("Normal Text"));
        QVERIFY(f.isValid());
        QVERIFY(f.isDefaultTextStyle(Theme()));
        QVERIFY(!f.hasTextColor(Theme()));
        QVERIFY(!f.hasBackgroundColor(Theme()));
    }
};
}

QTEST_GUILESS_MAIN(KSyntaxHighlighting::ThemeTest)

#include "theme_test.moc"

