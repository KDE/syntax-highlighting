/*
    SPDX-FileCopyrightText: 2021 Igor Kushnir <igorkuo@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef REPOSITORY_TEST_BASE_H
#define REPOSITORY_TEST_BASE_H

#include <KSyntaxHighlighting/Repository>

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

    void definitionForMimeTypeTestData();
    void definitionForMimeTypeTest(const QString &mimeTypeName, const QString &definitionName);
    void definitionsForMimeTypeTestData();
    void definitionsForMimeTypeTest(const QString &mimeTypeName, const QStringList &definitionNames);

    KSyntaxHighlighting::Repository m_repo;
};

#endif // REPOSITORY_TEST_BASE_H
