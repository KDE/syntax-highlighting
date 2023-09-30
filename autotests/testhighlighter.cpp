/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "test-config.h"

#include <KSyntaxHighlighting/AbstractHighlighter>
#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Format>
#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/State>
#include <KSyntaxHighlighting/Theme>

#include "state_p.h"

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
    int highlightFile(const QString &inFileName, const QString &outFileName)
    {
        QFile outFile(outFileName);
        if (!outFile.open(QFile::WriteOnly | QFile::Truncate)) {
            qWarning() << "Failed to open output file" << outFileName << ":" << outFile.errorString();
            return 0;
        }
        m_out.setDevice(&outFile);

        QFile f(inFileName);
        if (!f.open(QFile::ReadOnly)) {
            qWarning() << "Failed to open input file" << inFileName << ":" << f.errorString();
            return 0;
        }

        QTextStream in(&f);
        State state;
        while (in.readLineInto(&m_currentLine)) {
            state = highlightLine(m_currentLine, state);
            m_out << "<br/>\n";
        }

        m_out.flush();
        auto *stateData = StateData::get(state);
        return stateData ? stateData->size() : 0;
    }

protected:
    void applyFormat(int offset, int length, const Format &format) override
    {
        if (format.name().isEmpty()) {
            m_out << "<dsNormal>" << QStringView(m_currentLine).mid(offset, length) << "</dsNormal>";
        } else {
            m_out << "<" << format.name() << ">" << QStringView(m_currentLine).mid(offset, length) << "</" << format.name() << ">";
        }
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
            if (!def.isValid()) {
                continue;
            }
            ++count;
            if (m_coveredDefinitions.find(def.name()) != m_coveredDefinitions.end()) {
                coveredList.write(def.name().toUtf8() + '\n');
            } else {
                uncoveredList.write(def.name().toUtf8() + '\n');
            }
        }

        qDebug() << "Syntax definitions with test coverage:" << ((float)m_coveredDefinitions.size() * 100.0f / (float)count) << "%";

        // we don't want multiple tests for the same highlighting
        // tests should be consolidated into one useful file per highlighting
        // the update script for https://kate-editor.org/syntax/ will check that no duplicated output is there, too
        bool duplicates = false;
        for (const auto &entry : std::as_const(m_coveredDefinitions)) {
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
            if (inFile.endsWith(QLatin1String(".syntax"))) {
                continue;
            }

            QString syntax;
            QFile syntaxOverride(inFile + QStringLiteral(".syntax"));
            if (syntaxOverride.exists() && syntaxOverride.open(QFile::ReadOnly)) {
                syntax = QString::fromUtf8(syntaxOverride.readAll()).trimmed();
            }

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
        if (!syntax.isEmpty()) {
            def = m_repo->definitionForName(syntax);
        }

        TestHighlighter highlighter;
        highlighter.setTheme(m_repo->defaultTheme());
        QVERIFY(highlighter.theme().isValid());

        QVERIFY(def.isValid());
        qDebug() << "Using syntax" << def.name();
        m_coveredDefinitions[def.name()].push_back(inFile);
        highlighter.setDefinition(def);
        int lastStackSize = highlighter.highlightFile(inFile, outFile);
        // arbitrary condition to detect syntaxes that stack contexts indefinitely
        // instead of going back to the parent
        QVERIFY2(lastStackSize < 15, qPrintable(def.name() + QStringLiteral("seems to stack contexts indefinitely, perhaps #pop's are missing?")));

        /**
         * compare results
         */
        compareFiles(refFile, outFile);
    }
};

QTEST_GUILESS_MAIN(TestHighlighterTest)

#include "testhighlighter.moc"
