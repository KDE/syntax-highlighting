/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "test-config.h"

#include <repository.h>

#include <QObject>
#include <QTest>

using namespace KSyntaxHighlighting;

class RepositoryBenchmark : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void benchmarkReporitoryLoad()
    {
        QBENCHMARK {
            Repository repo;
            initRepositorySearchPaths(repo);
        }
    }
};

QTEST_GUILESS_MAIN(RepositoryBenchmark)

#include "repository_benchmark.moc"
