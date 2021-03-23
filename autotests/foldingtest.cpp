/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
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
#include <QStandardPaths>
#include <QTest>
#include <QTextStream>

#include <unordered_map>

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
            for (const auto &fold : qAsConst(m_folds)) {
                // use stable ids for output, see below docs for m_stableFoldingIds
                const auto stableId = m_stableFoldingIds[fold.region.id()];
                m_out << currentLine.mid(offset, fold.offset - offset);
                if (fold.region.type() == FoldingRegion::Begin)
                    m_out << "<beginfold id='" << stableId << "'>";
                else
                    m_out << "<endfold id='" << stableId << "'>";
                m_out << currentLine.mid(fold.offset, fold.length);
                if (fold.region.type() == FoldingRegion::Begin)
                    m_out << "</beginfold id='" << stableId << "'>";
                else
                    m_out << "</endfold id='" << stableId << "'>";
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

        // create stable id if needed, see below m_stableFoldingIds docs for details
        // start with 1
        m_stableFoldingIds.emplace(region.id(), m_stableFoldingIds.size() + 1);
    }

private:
    QTextStream m_out;
    struct Fold {
        int offset;
        int length;
        FoldingRegion region;
    };
    QVector<Fold> m_folds;

    // we use one repository for all tests
    // => the folding ids might change even if just unrelated highlighings are added
    // => construct some stable id per test based on occurrence of id
    std::unordered_map<uint32_t, size_t> m_stableFoldingIds;
};

class FoldingTest : public QObject
{
    Q_OBJECT
public:
    explicit FoldingTest(QObject *parent = nullptr)
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

    void testFolding_data()
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

            QTest::newRow(fileName.toUtf8().constData()) << inFile << (QStringLiteral(TESTBUILDDIR "/folding.out/") + fileName + QStringLiteral(".fold"))
                                                         << (QStringLiteral(TESTSRCDIR "/folding/") + fileName + QStringLiteral(".fold")) << syntax;
        }

        // cleanup before we test
        QDir(QStringLiteral(TESTBUILDDIR "/folding.out/")).removeRecursively();
        QDir().mkpath(QStringLiteral(TESTBUILDDIR "/folding.out/"));
    }

    void testFolding()
    {
        QFETCH(QString, inFile);
        QFETCH(QString, outFile);
        QFETCH(QString, refFile);
        QFETCH(QString, syntax);
        QVERIFY(m_repo);

        auto def = m_repo->definitionForFileName(inFile);
        if (!syntax.isEmpty())
            def = m_repo->definitionForName(syntax);

        FoldingHighlighter highlighter;
        QVERIFY(def.isValid());
        highlighter.setDefinition(def);
        highlighter.highlightFile(inFile, outFile);

        /**
         * compare results
         */
        compareFiles(refFile, outFile);
    }
};

QTEST_GUILESS_MAIN(FoldingTest)

#include "foldingtest.moc"
