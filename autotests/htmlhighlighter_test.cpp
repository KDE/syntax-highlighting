/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "test-config.h"

#include <definition.h>
#include <htmlhighlighter.h>
#include <repository.h>
#include <theme.h>

#include <QDir>
#include <QObject>
#include <QStandardPaths>
#include <QTest>

using namespace KSyntaxHighlighting;

class HTMLHighlighterTest : public QObject
{
    Q_OBJECT
public:
    explicit HTMLHighlighterTest(QObject *parent = nullptr)
        : QObject(parent)
        , m_repo(nullptr)
    {
    }

private:
    Repository *m_repo;

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
        m_repo = new Repository;
        initRepositorySearchPaths(*m_repo);
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
        QTest::addColumn<QString>("outDarkFile");
        QTest::addColumn<QString>("refDarkFile");
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

            QTest::newRow(fileName.toUtf8().constData()) << inFile << (QStringLiteral(TESTBUILDDIR "/html.output/") + fileName + QStringLiteral(".html"))
                                                         << (QStringLiteral(TESTSRCDIR "/html/") + fileName + QStringLiteral(".html"))
                                                         << (QStringLiteral(TESTBUILDDIR "/html.output/") + fileName + QStringLiteral(".dark.html"))
                                                         << (QStringLiteral(TESTSRCDIR "/html/") + fileName + QStringLiteral(".dark.html")) << syntax;
        }

        // cleanup before we test
        QDir(QStringLiteral(TESTBUILDDIR "/html.output/")).removeRecursively();
        QDir().mkpath(QStringLiteral(TESTBUILDDIR "/html.output/"));
    }

    void testHighlight()
    {
        QFETCH(QString, inFile);
        QFETCH(QString, outFile);
        QFETCH(QString, refFile);
        QFETCH(QString, outDarkFile);
        QFETCH(QString, refDarkFile);
        QFETCH(QString, syntax);
        QVERIFY(m_repo);

        // get the matching definitions
        const auto def = syntax.isEmpty() ? m_repo->definitionForFileName(inFile) : m_repo->definitionForName(syntax);
        QVERIFY(def.isValid());

        // we try both light and dark themes
        for (int i = 0; i < 2; ++i) {
            HtmlHighlighter highlighter;
            highlighter.setTheme(m_repo->defaultTheme((i == 0) ? Repository::LightTheme : Repository::DarkTheme));
            QVERIFY(highlighter.theme().isValid());
            highlighter.setDefinition(def);
            highlighter.setOutputFile((i == 0) ? outFile : outDarkFile);
            highlighter.highlightFile(inFile);

            // compare results
            compareFiles((i == 0) ? refFile : refDarkFile, (i == 0) ? outFile : outDarkFile);
        }
    }
};

QTEST_GUILESS_MAIN(HTMLHighlighterTest)

#include "htmlhighlighter_test.moc"
