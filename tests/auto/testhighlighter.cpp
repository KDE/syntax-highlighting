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
#include <abstracthighlighter.h>

#include <QDirIterator>
#include <QFile>
#include <QObject>
#include <QProcess>
#include <QTextStream>
#include <QtTest/qtest.h>

using namespace KateSyntax;

class TestHighlighter : public AbstractHighlighter
{
public:
    void highlightFile(const QString &inFileName, const QString &outFileName)
    {
        QFile outFile(outFileName);
        if (!outFile.open(QFile::WriteOnly | QFile::Truncate)) {
            qWarning() << "Failed to open output file" << outFileName << ":" << outFile.errorString();
            return;
        }
        m_out.setDevice(&outFile);

        QFile f(inFileName);
        if (!f.open(QFile::ReadOnly)) {
            qWarning() << "Failed to open input file" << inFileName << ":" << f.errorString();
            return;
        }

        QTextStream in(&f);
        while (!in.atEnd()) {
            m_currentLine = in.readLine();
            highlightLine(m_currentLine);
            m_out << "<br/>\n";
        }

        m_out.flush();
    }

protected:
    void setFormat(int offset, int length, const Format &format) override
    {
        if (format.name().isEmpty())
            m_out << "<dsNormal>" << m_currentLine.midRef(offset, length) << "</dsNormal>";
        else
            m_out << "<" << format.name() << ">" << m_currentLine.midRef(offset, length) << "</" << format.name() << ">";
    }

private:
    QTextStream m_out;
    QString m_currentLine;
};


class TestHighlighterTest : public QObject
{
    Q_OBJECT
public:
    explicit TestHighlighterTest(QObject *parent = nullptr) : QObject(parent), m_repo(nullptr) {}
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

            QTest::newRow(it.fileName().toUtf8()) << inFile
                << (QStringLiteral(TESTBUILDDIR "/output/") + it.fileName() + QStringLiteral(".ref"))
                << (QStringLiteral(TESTSRCDIR "/reference/") + it.fileName() + QStringLiteral(".ref"))
                << syntax;
        }

        QDir().mkpath(QStringLiteral(TESTBUILDDIR "/output/"));
    }

    void testHighlight()
    {
        QFETCH(QString, inFile);
        QFETCH(QString, outFile);
        QFETCH(QString, refFile);
        QFETCH(QString, syntax);
        QVERIFY(m_repo);

        auto def = m_repo->definitionForFileName(inFile);
        if (!syntax.isEmpty())
            def = m_repo->definitionForName(syntax);

        TestHighlighter highlighter;
        QVERIFY(def);
        qDebug() << "Using syntax" << def->name();
        highlighter.setDefinition(def);
        highlighter.highlightFile(inFile, outFile);

        auto args = QStringList() << QStringLiteral("-u") << refFile << outFile;
        QProcess proc;
        proc.setProcessChannelMode(QProcess::ForwardedChannels);
        proc.start(QStringLiteral("diff"), args);
        QVERIFY(proc.waitForFinished());
        QCOMPARE(proc.exitCode(), 0);
    }

};

QTEST_MAIN(TestHighlighterTest)

#include "testhighlighter.moc"


