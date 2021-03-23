/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "test-config.h"

#include <abstracthighlighter.h>
#include <definition.h>
#include <format.h>
#include <repository.h>
#include <state.h>
#include <theme.h>

#include <QDir>
#include <QFile>
#include <QObject>
#include <QTest>
#include <QTextStream>

#include <map>

using namespace KSyntaxHighlighting;

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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        m_out.setCodec("UTF-8");
#endif

        QFile f(inFileName);
        if (!f.open(QFile::ReadOnly)) {
            qWarning() << "Failed to open input file" << inFileName << ":" << f.errorString();
            return;
        }

        QTextStream in(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        in.setCodec("UTF-8");
#endif
        State state;
        while (!in.atEnd()) {
            m_currentLine = in.readLine();
            state = highlightLine(m_currentLine, state);
            m_out << "<br/>\n";
        }

        m_out.flush();
    }

protected:
    void applyFormat(int offset, int length, const Format &format) Q_DECL_OVERRIDE
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
    explicit TestHighlighterTest(QObject *parent = nullptr)
        : QObject(parent)
        , m_repo(nullptr)
    {
    }

private:
    Repository *m_repo;
    std::map<QString, QStringList> m_coveredDefinitions;

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
        m_repo = new Repository;
        initRepositorySearchPaths(*m_repo);
    }

    void cleanupTestCase()
    {
        QFile coveredList(QLatin1String(TESTBUILDDIR "/covered-definitions.txt"));
        QFile uncoveredList(QLatin1String(TESTBUILDDIR "/uncovered-definition.txt"));
        QVERIFY(coveredList.open(QFile::WriteOnly));
        QVERIFY(uncoveredList.open(QFile::WriteOnly));

        int count = 0;
        for (const auto &def : m_repo->definitions()) {
            if (!def.isValid())
                continue;
            ++count;
            if (m_coveredDefinitions.find(def.name()) != m_coveredDefinitions.end())
                coveredList.write(def.name().toUtf8() + '\n');
            else
                uncoveredList.write(def.name().toUtf8() + '\n');
        }

        qDebug() << "Syntax definitions with test coverage:" << ((float)m_coveredDefinitions.size() * 100.0f / (float)count) << "%";

        // we don't want multiple tests for the same highlighting
        // tests should be consolidated into one useful file per highlighting
        // the update script for https://kate-editor.org/syntax/ will check that no duplicated output is there, too
        bool duplicates = false;
        for (const auto &entry : qAsConst(m_coveredDefinitions)) {
            if (entry.second.size() <= 1) {
                continue;
            }

            // abort and tell about duplicated test cases!
            qWarning() << "Multiple unit tests for the language " << entry.first;
            for (const auto &testCase : entry.second) {
                qWarning() << "  - " << testCase;
            }
            duplicates = true;
        }
        if (duplicates) {
            QFAIL("Multiple unit tests for the same language found, see for details the output above!");
        }

        delete m_repo;
        m_repo = nullptr;
    }

    void testHighlight_data()
    {
        QTest::addColumn<QString>("inFile");
        QTest::addColumn<QString>("outFile");
        QTest::addColumn<QString>("refFile");
        QTest::addColumn<QString>("syntax");

        const QDir dir(QStringLiteral(TESTSRCDIR "/input"));
        for (const auto &fileName : dir.entryList(QDir::Files | QDir::NoSymLinks | QDir::Readable | QDir::Hidden, QDir::Name)) {
            // skip .clang-format file we use to avoid formatting test files
            if (fileName == QLatin1String(".clang-format")) {
                continue;
            }

            const auto inFile = dir.absoluteFilePath(fileName);
            if (inFile.endsWith(QLatin1String(".syntax")))
                continue;

            QString syntax;
            QFile syntaxOverride(inFile + QStringLiteral(".syntax"));
            if (syntaxOverride.exists() && syntaxOverride.open(QFile::ReadOnly))
                syntax = QString::fromUtf8(syntaxOverride.readAll()).trimmed();

            QTest::newRow(fileName.toUtf8().constData()) << inFile << (QStringLiteral(TESTBUILDDIR "/output/") + fileName + QStringLiteral(".ref"))
                                                         << (QStringLiteral(TESTSRCDIR "/reference/") + fileName + QStringLiteral(".ref")) << syntax;
        }

        // cleanup before we test
        QDir(QStringLiteral(TESTBUILDDIR "/output/")).removeRecursively();
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
        highlighter.setTheme(m_repo->defaultTheme());
        QVERIFY(highlighter.theme().isValid());

        QVERIFY(def.isValid());
        qDebug() << "Using syntax" << def.name();
        m_coveredDefinitions[def.name()].push_back(inFile);
        highlighter.setDefinition(def);
        highlighter.highlightFile(inFile, outFile);

        /**
         * compare results
         */
        compareFiles(refFile, outFile);
    }
};

QTEST_GUILESS_MAIN(TestHighlighterTest)

#include "testhighlighter.moc"
