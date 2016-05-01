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

#include "test-config.h"

#include <syntaxrepository.h>
#include <syntaxdefinition.h>
#include <htmlhighlighter.h>

#include <QObject>
#include <QtTest/qtest.h>

using namespace KateSyntax;

class HighlighterBenchmark : public QObject
{
    Q_OBJECT
public:
    explicit HighlighterBenchmark(QObject *parent = nullptr) : QObject(parent), m_repo(nullptr) {}

private:
    SyntaxRepository *m_repo;

private slots:
    void initTestCase()
    {
        m_repo = new SyntaxRepository;
    }

    void cleanupTestCase()
    {
        delete m_repo;
        m_repo = nullptr;
    }

    void benchmarkHighlight_data()
    {
        QTest::addColumn<QString>("inFile");
        QTest::addColumn<QString>("syntax");

        QTest::newRow("mimedb") << QStringLiteral(":/qt-project.org/qmime/freedesktop.org.xml") << QStringLiteral("XML");
    }

    void benchmarkHighlight()
    {
        QFETCH(QString, inFile);
        QFETCH(QString, syntax);
        QVERIFY(m_repo);

        HtmlHighlighter highlighter;
        auto def = m_repo->definitionForFileName(inFile);
        if (!syntax.isEmpty())
            def = m_repo->definitionForName(syntax);
        QVERIFY(def);
        highlighter.setDefinition(def);
        highlighter.setOutputFile(QStringLiteral("/dev/null"));
        QBENCHMARK {
            highlighter.highlightFile(inFile);
        }
    }

};

QTEST_MAIN(HighlighterBenchmark)

#include "highlighter_benchmark.moc"

