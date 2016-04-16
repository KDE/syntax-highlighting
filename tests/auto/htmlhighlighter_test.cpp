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

#include <QDirIterator>
#include <QObject>
#include <QProcess>
#include <QtTest/qtest.h>

using namespace KateSyntax;

class HTMLHighlighterTest : public QObject
{
    Q_OBJECT
private:
        SyntaxRepository m_repo;

private slots:
    void testHighlight_data()
    {
        QTest::addColumn<QString>("inFile");
        QTest::addColumn<QString>("outFile");
        QTest::addColumn<QString>("refFile");

        QDirIterator it(QStringLiteral(TESTSRCDIR "/input"), QDir::Files | QDir::NoSymLinks | QDir::Readable);
        while (it.hasNext()) {
            const auto inFile = it.next();
            QTest::newRow(it.fileName().toUtf8()) << inFile
                << (QStringLiteral(TESTBUILDDIR "/output/") + it.fileName() + QStringLiteral(".html"))
                << (QStringLiteral(TESTSRCDIR "/reference/") + it.fileName() + QStringLiteral(".ref.html"));
        }

        QDir().mkpath(QStringLiteral(TESTBUILDDIR "/output/"));
    }

    void testHighlight()
    {
        QFETCH(QString, inFile);
        QFETCH(QString, outFile);
        QFETCH(QString, refFile);

        qDebug() << inFile << outFile << refFile;

        HTMLHighlighter highlighter;
        highlighter.setDefinition(m_repo.definitionForFileName(inFile));
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

