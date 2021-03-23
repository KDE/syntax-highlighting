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

#include <QDir>
#include <QObject>
#include <QTest>

using namespace KSyntaxHighlighting;

class NullHighlighter : public AbstractHighlighter
{
public:
    /**
     * Read in the given file and cache it for the highlighting benchmarking
     * @param inFileName file to read
     */
    NullHighlighter(const QString &inFileName)
    {
        QFile f(inFileName);
        if (!f.open(QFile::ReadOnly)) {
            qWarning() << "Failed to open input file" << inFileName << ":" << f.errorString();
            return;
        }

        QTextStream in(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        in.setCodec("UTF-8");
#endif
        while (!in.atEnd())
            m_fileContents.append(in.readLine());
    }

    /**
     * highlight the in-memory stored file
     * @return number of highlighted lines
     */
    int highlightFile()
    {
        State state;
        for (const auto &line : qAsConst(m_fileContents))
            state = highlightLine(line, state);
        return m_fileContents.size();
    }

protected:
    void applyFormat(int, int, const Format &) Q_DECL_OVERRIDE
    {
    }
    QStringList m_fileContents;
};

class HighlighterBenchmark : public QObject
{
    Q_OBJECT
public:
    explicit HighlighterBenchmark(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    Repository m_repo;

private Q_SLOTS:
    void initTestCase()
    {
        initRepositorySearchPaths(m_repo);
    }

    void cleanupTestCase()
    {
    }

    void benchmarkHighlight_data()
    {
        QTest::addColumn<QString>("inFile");
        QTest::addColumn<QString>("syntax");

        const QDir dir(QStringLiteral(TESTSRCDIR "/input"));
        for (const auto &fileName : dir.entryList(QDir::Files | QDir::NoSymLinks | QDir::Readable | QDir::Hidden, QDir::Name)) {
            if (fileName == QLatin1String(".clang-format"))
                continue;

            const auto inFile = dir.absoluteFilePath(fileName);
            if (inFile.endsWith(QLatin1String(".syntax")))
                continue;

            QString syntax;
            QFile syntaxOverride(inFile + QStringLiteral(".syntax"));
            if (syntaxOverride.exists() && syntaxOverride.open(QFile::ReadOnly))
                syntax = QString::fromUtf8(syntaxOverride.readAll()).trimmed();

            QTest::newRow(fileName.toUtf8().constData()) << inFile << syntax;
        }
    }

    void benchmarkHighlight()
    {
        QFETCH(QString, inFile);
        QFETCH(QString, syntax);

        NullHighlighter highlighter(inFile);
        auto def = m_repo.definitionForFileName(inFile);
        if (!syntax.isEmpty())
            def = m_repo.definitionForName(syntax);
        QVERIFY(def.isValid());
        highlighter.setDefinition(def);

        // trigger loading of definition per benchmarking loop
        QVERIFY(!def.formats().isEmpty());

        // benchmark the highlighting
        // try to highlight ~ 20000 lines per file
        // bail out, if file is empty, else we are stuck
        for (int i = 0; i <= 20000;) {
            int lines = highlighter.highlightFile();
            if (lines <= 0)
                break;
            i += lines;
        }
    }
};

QTEST_GUILESS_MAIN(HighlighterBenchmark)

#include "highlighter_benchmark.moc"
