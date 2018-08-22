/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "test-config.h"

#include <abstracthighlighter.h>
#include <definition.h>
#include <repository.h>
#include <state.h>

#include <QObject>
#include <qtest.h>

using namespace KSyntaxHighlighting;

class NullHighlighter : public AbstractHighlighter
{
public:
    void highlightFile(const QString &inFileName)
    {
        QFile f(inFileName);
        if (!f.open(QFile::ReadOnly)) {
            qWarning() << "Failed to open input file" << inFileName << ":" << f.errorString();
            return;
        }

        QTextStream in(&f);
        State state;
        while (!in.atEnd())
            state = highlightLine(in.readLine(), state);
    }

protected:
    void applyFormat(int, int, const Format&) Q_DECL_OVERRIDE {}
};

class HighlighterBenchmark : public QObject
{
    Q_OBJECT
public:
    explicit HighlighterBenchmark(QObject *parent = nullptr) : QObject(parent), m_repo(nullptr) {}

private:
    Repository *m_repo;

private Q_SLOTS:
    void initTestCase()
    {
        m_repo = new Repository;
        initRepositorySearchPaths(*m_repo);
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

        NullHighlighter highlighter;
        auto def = m_repo->definitionForFileName(inFile);
        if (!syntax.isEmpty())
            def = m_repo->definitionForName(syntax);
        QVERIFY(def.isValid());
        highlighter.setDefinition(def);
        QBENCHMARK {
            highlighter.highlightFile(inFile);
        }
    }

};

QTEST_GUILESS_MAIN(HighlighterBenchmark)

#include "highlighter_benchmark.moc"

