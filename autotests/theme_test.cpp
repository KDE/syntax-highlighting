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
#include <QtTest/qtest.h>

namespace SyntaxHighlighting {

class FormatCollector : public AbstractHighlighter
{
public:
    using AbstractHighlighter::highlightLine;
    void setFormat(int offset, int length, const Format &format) Q_DECL_OVERRIDE
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
        Q_FOREACH (const auto theme, m_repo.themes()) {
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

        // visually identical to normal text
        f = collector.formatMap.value(QLatin1String("Symbol"));
        QVERIFY(f.isValid());
        QVERIFY(f.isDefaultTextStyle(t));
        QVERIFY(!f.hasTextColor(t));

        // visually different to normal text
        f = collector.formatMap.value(QLatin1String("Keywords"));
        QVERIFY(f.isValid());
        QVERIFY(!f.isDefaultTextStyle(t));
        QVERIFY(f.isBold(t));

        f = collector.formatMap.value(QLatin1String("Float"));
        QVERIFY(f.isValid());
        QVERIFY(!f.isDefaultTextStyle(t));
        QVERIFY(f.hasTextColor(t));
    }

    void testDefaultTheme()
    {
        Theme t = m_repo.theme(QLatin1String("Default"));
        QVERIFY(t.isValid());

        QCOMPARE(t.areaBackgroundColor()   , QColor("#ffffff").rgb());
        QCOMPARE(t.codeFoldingColor()      , QColor("#94caef").rgb());
        QCOMPARE(t.brackedMatchingColor()  , QColor("#ffff00").rgb());
        QCOMPARE(t.currentLineColor    ()  , QColor("#f8f7f6").rgb());
        QCOMPARE(t.iconBorderColor()       , QColor("#f0f0f0").rgb());
        QCOMPARE(t.indentationLineColor()  , QColor("#d2d2d2").rgb());
        QCOMPARE(t.lineNumberColor()       , QColor("#a0a0a0").rgb());
        QCOMPARE(t.currentLineNumberColor(), QColor("#1e1e1e").rgb());
    }
};
}

QTEST_GUILESS_MAIN(SyntaxHighlighting::ThemeTest)

#include "theme_test.moc"

