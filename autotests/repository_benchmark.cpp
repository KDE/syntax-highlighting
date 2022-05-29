/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2021 Igor Kushnir <igorkuo@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "repository_test_base.h"
#include "test-config.h"

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Repository>

#include <QTest>

class RepositoryBenchmark : public RepositoryTestBase
{
    Q_OBJECT
private Q_SLOTS:
    void benchmarkRepositoryLoad()
    {
        QBENCHMARK {
            KSyntaxHighlighting::Repository repo;
            initRepositorySearchPaths(repo);
        }
    }

    void benchmarkDefinitionByExtension_data()
    {
        definitionByExtensionTestData();
    }

    void benchmarkDefinitionByExtension()
    {
        QFETCH(QString, fileName);
        QFETCH(QString, definitionName);

        // Warm up and check correctness.
        definitionByExtensionTest(fileName, definitionName);
        if (QTest::currentTestFailed())
            return;

        QBENCHMARK {
            m_repo.definitionForFileName(fileName);
        }
    }

    void benchmarkDefinitionsForFileName_data()
    {
        definitionsForFileNameTestData();
    }

    void benchmarkDefinitionsForFileName()
    {
        QFETCH(QString, fileName);
        QFETCH(QStringList, definitionNames);

        // Warm up and check correctness.
        definitionsForFileNameTest(fileName, definitionNames);
        if (QTest::currentTestFailed())
            return;

        QBENCHMARK {
            m_repo.definitionsForFileName(fileName);
        }
    }

    void benchmarkDefinitionForMimeType_data()
    {
        definitionForMimeTypeTestData();
    }

    void benchmarkDefinitionForMimeType()
    {
        QFETCH(QString, mimeTypeName);
        QFETCH(QString, definitionName);

        // Warm up and check correctness.
        definitionForMimeTypeTest(mimeTypeName, definitionName);
        if (QTest::currentTestFailed())
            return;

        QBENCHMARK {
            m_repo.definitionForMimeType(mimeTypeName);
        }
    }

    void benchmarkDefinitionsForMimeType_data()
    {
        definitionsForMimeTypeTestData();
    }

    void benchmarkDefinitionsForMimeType()
    {
        QFETCH(QString, mimeTypeName);
        QFETCH(QStringList, definitionNames);

        // Warm up and check correctness.
        definitionsForMimeTypeTest(mimeTypeName, definitionNames);
        if (QTest::currentTestFailed())
            return;

        QBENCHMARK {
            m_repo.definitionsForMimeType(mimeTypeName);
        }
    }
};

QTEST_GUILESS_MAIN(RepositoryBenchmark)

#include "repository_benchmark.moc"
