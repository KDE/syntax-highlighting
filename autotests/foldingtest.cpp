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

#include <abstracthighlighter.h>
#include <definition.h>
#include <foldingregion.h>
#include <repository.h>
#include <state.h>

#include <QDir>
#include <QFile>
#include <QObject>
#include <QProcess>
#include <QStandardPaths>
#include <QTextStream>
#include <QtTest/qtest.h>

using namespace KSyntaxHighlighting;

class FoldingHighlighter : public AbstractHighlighter
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
        m_out.setCodec("UTF-8");

        QFile f(inFileName);
        if (!f.open(QFile::ReadOnly)) {
            qWarning() << "Failed to open input file" << inFileName << ":" << f.errorString();
            return;
        }

        QTextStream in(&f);
        in.setCodec("UTF-8");
        State state;
        bool indentationFoldEnabled = definition().indentationBasedFoldingEnabled();
        if (indentationFoldEnabled)
            m_out << "<indentfold>";
        while (!in.atEnd()) {
            const auto currentLine = in.readLine();
            state = highlightLine(currentLine, state);

            if (indentationFoldEnabled != state.indentationBasedFoldingEnabled()) {
                indentationFoldEnabled = state.indentationBasedFoldingEnabled();
                if (indentationFoldEnabled)
                    m_out << "<indentfold>";
                else
                    m_out << "</indentfold>";
            }

            int offset = 0;
            foreach (const auto &fold, m_folds) {
                m_out << currentLine.mid(offset, fold.offset - offset);
                if (fold.region.type() == FoldingRegion::Begin)
                    m_out << "<beginfold id='" << fold.region.id() << "'>";
                else
                    m_out << "<endfold id='" << fold.region.id() << "'>";
                m_out << currentLine.mid(fold.offset, fold.length);
                if (fold.region.type() == FoldingRegion::Begin)
                    m_out << "</beginfold id='" << fold.region.id() << "'>";
                else
                    m_out << "</endfold id='" << fold.region.id() << "'>";
                offset = fold.offset + fold.length;
            }
            m_out << currentLine.mid(offset) << '\n';
            m_folds.clear();
        }

        m_out.flush();
    }

protected:
    void applyFormat(int offset, int length, const Format &format) Q_DECL_OVERRIDE
    {
        Q_UNUSED(offset);
        Q_UNUSED(length);
        Q_UNUSED(format);
    }

    void applyFolding(int offset, int length, FoldingRegion region) Q_DECL_OVERRIDE
    {
        Q_ASSERT(region.isValid());
        m_folds.push_back({offset, length, region});
    }

private:
    QTextStream m_out;
    struct Fold { int offset; int length; FoldingRegion region; };
    QVector<Fold> m_folds;
};


class FoldingTest : public QObject
{
    Q_OBJECT
public:
    explicit FoldingTest(QObject *parent = Q_NULLPTR) : QObject(parent) {}
private:

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::enableTestMode(true);
    }

    void testFolding_data()
    {
        QTest::addColumn<QString>("inFile");
        QTest::addColumn<QString>("outFile");
        QTest::addColumn<QString>("refFile");
        QTest::addColumn<QString>("syntax");

        QDir dir(QStringLiteral(TESTSRCDIR "/input"));
        foreach (const auto &fileName, dir.entryList(QDir::Files | QDir::NoSymLinks | QDir::Readable, QDir::Name)) {
            const auto inFile = dir.absoluteFilePath(fileName);
            if (inFile.endsWith(QLatin1String(".syntax")))
                continue;

            QString syntax;
            QFile syntaxOverride(inFile + QStringLiteral(".syntax"));
            if (syntaxOverride.exists() && syntaxOverride.open(QFile::ReadOnly))
                syntax = QString::fromUtf8(syntaxOverride.readAll()).trimmed();

            QTest::newRow(fileName.toUtf8().constData()) << inFile
                << (QStringLiteral(TESTBUILDDIR "/folding.out/") + fileName + QStringLiteral(".fold"))
                << (QStringLiteral(TESTSRCDIR "/folding/") + fileName + QStringLiteral(".fold"))
                << syntax;
        }

        QDir().mkpath(QStringLiteral(TESTBUILDDIR "/folding.out/"));
    }

    void testFolding()
    {
        QFETCH(QString, inFile);
        QFETCH(QString, outFile);
        QFETCH(QString, refFile);
        QFETCH(QString, syntax);

        Repository m_repo;

        auto def = m_repo.definitionForFileName(inFile);
        if (!syntax.isEmpty())
            def = m_repo.definitionForName(syntax);

        FoldingHighlighter highlighter;

        QVERIFY(def.isValid());
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

QTEST_GUILESS_MAIN(FoldingTest)

#include "foldingtest.moc"
