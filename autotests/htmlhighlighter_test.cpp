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

#include <repository.h>
#include <syntaxdefinition.h>
#include <htmlhighlighter.h>

#include <QDirIterator>
#include <QObject>
#include <QProcess>
#include <QtTest/qtest.h>

using namespace SyntaxHighlighting;

class HTMLHighlighterTest : public QObject
{
    Q_OBJECT
public:
    explicit HTMLHighlighterTest(QObject *parent = nullptr) : QObject(parent), m_repo(nullptr) {}

private:
    Repository *m_repo;

private Q_SLOTS:
    void initTestCase()
    {
        m_repo = new Repository;
    }

    void cleanupTestCase()
    {
        delete m_repo;
        m_repo = nullptr;
    }

    void testHighlight_data()
    {
        QTest::addColumn<QString>("inFile");
        QTest::addColumn<QString>("outFile");
        QTest::addColumn<QString>("refFile");
        QTest::addColumn<QString>("syntax");

        QDirIterator it(QStringLiteral(TESTSRCDIR "/input"), QDir::Files | QDir::NoSymLinks | QDir::Readable);
        while (it.hasNext()) {
            const auto inFile = it.next();
            if (inFile.endsWith(QLatin1String(".syntax")))
                continue;

            QString syntax;
            QFile syntaxOverride(inFile + QStringLiteral(".syntax"));
            if (syntaxOverride.exists() && syntaxOverride.open(QFile::ReadOnly))
                syntax = QString::fromUtf8(syntaxOverride.readAll()).trimmed();


            QTest::newRow(it.fileName().toUtf8().constData()) << inFile
                << (QStringLiteral(TESTBUILDDIR "/html.output/") + it.fileName() + QStringLiteral(".html"))
                << (QStringLiteral(TESTSRCDIR "/html/") + it.fileName() + QStringLiteral(".html"))
                << syntax;
        }

        QDir().mkpath(QStringLiteral(TESTBUILDDIR "/html.output/"));
    }

    void testHighlight()
    {
        QFETCH(QString, inFile);
        QFETCH(QString, outFile);
        QFETCH(QString, refFile);
        QFETCH(QString, syntax);
        QVERIFY(m_repo);

        HtmlHighlighter highlighter;
        auto def = m_repo->definitionForFileName(inFile);
        if (!syntax.isEmpty())
            def = m_repo->definitionForName(syntax);
        QVERIFY(def.isValid());
        highlighter.setDefinition(def);
        highlighter.setOutputFile(outFile);
        highlighter.highlightFile(inFile);

        auto args = QStringList() << QStringLiteral("-u") << refFile << outFile;
        QProcess proc;
        proc.setProcessChannelMode(QProcess::ForwardedChannels);
        proc.start(QStringLiteral("diff"), args);
        QVERIFY(proc.waitForFinished());
        QCOMPARE(proc.exitCode(), 0);
    }

};

QTEST_MAIN(HTMLHighlighterTest)

#include "htmlhighlighter_test.moc"

