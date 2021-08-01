/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2021 Igor Kushnir <igorkuo@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "repository_test_base.h"

#include "test-config.h"

#include <definition.h>
#include <repository.h>

#include <QStandardPaths>
#include <QTest>

void RepositoryTestBase::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
    initRepositorySearchPaths(m_repo);
}

void RepositoryTestBase::definitionByExtensionTestData()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("definitionName");

    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("qml") << QStringLiteral("/bla/foo.qml") << QStringLiteral("QML");
    QTest::newRow("glsl") << QStringLiteral("flat.frag") << QStringLiteral("GLSL");
    // the following ones are defined in multiple syntax definitions
    QTest::newRow("c") << QStringLiteral("test.c") << QStringLiteral("C");
    QTest::newRow("c++") << QStringLiteral("test.cpp") << QStringLiteral("C++");
    QTest::newRow("fs") << QStringLiteral("test.fs") << QStringLiteral("FSharp");
    QTest::newRow("markdown") << QStringLiteral("test.md") << QStringLiteral("Markdown");

    QTest::newRow("Makefile 1") << QStringLiteral("Makefile") << QStringLiteral("Makefile");
    QTest::newRow("Makefile 2") << QStringLiteral("/some/path/to/Makefile") << QStringLiteral("Makefile");
    QTest::newRow("Makefile 3") << QStringLiteral("Makefile.am") << QStringLiteral("Makefile");
}

void RepositoryTestBase::definitionByExtensionTest(const QString &fileName, const QString &definitionName)
{
    const auto definition = m_repo.definitionForFileName(fileName);
    if (definitionName.isEmpty()) {
        QVERIFY(!definition.isValid());
    } else {
        QVERIFY(definition.isValid());
        QCOMPARE(definition.name(), definitionName);
    }
}

void RepositoryTestBase::definitionsForFileNameTestData()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QStringList>("definitionNames");

    QTest::newRow("Matlab") << QStringLiteral("/bla/foo.m")
                            << QStringList{QStringLiteral("Objective-C"), QStringLiteral("Magma"), QStringLiteral("Matlab"), QStringLiteral("Octave")};
}

void RepositoryTestBase::definitionsForFileNameTest(const QString &fileName, const QStringList &definitionNames)
{
    const auto definitions = m_repo.definitionsForFileName(fileName);
    QStringList names;
    for (const auto &definition : definitions) {
        names.push_back(definition.name());
    }
    QCOMPARE(names, definitionNames);
}
