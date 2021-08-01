/*
    SPDX-FileCopyrightText: 2021 Igor Kushnir <igorkuo@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef REPOSITORY_TEST_BASE_H
#define REPOSITORY_TEST_BASE_H

#include <repository.h>

#include <QObject>
#include <QString>
#include <QStringList>

class RepositoryTestBase : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

protected:
    void definitionByExtensionTestData();
    void definitionByExtensionTest(const QString &fileName, const QString &definitionName);
    void definitionsForFileNameTestData();
    void definitionsForFileNameTest(const QString &fileName, const QStringList &definitionNames);

    KSyntaxHighlighting::Repository m_repo;
};

#endif // REPOSITORY_TEST_BASE_H
