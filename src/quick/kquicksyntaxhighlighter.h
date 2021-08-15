/*
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KQUICKSYNTAXHIGHLIGHTER_H
#define KQUICKSYNTAXHIGHLIGHTER_H

#include "repositorywrapper.h"

#include <QObject>

namespace KSyntaxHighlighting
{
class Repository;
class SyntaxHighlighter;
}

class KQuickSyntaxHighlighter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject *textEdit READ textEdit WRITE setTextEdit NOTIFY textEditChanged)
    Q_PROPERTY(QString formatName READ formatName WRITE setFormatName NOTIFY formatNameChanged)
    Q_PROPERTY(RepositoryWrapper *repository READ repository WRITE setRepository NOTIFY repositoryChanged)

public:
    explicit KQuickSyntaxHighlighter(QObject *parent = nullptr);
    ~KQuickSyntaxHighlighter() override;

    QObject *textEdit() const;
    void setTextEdit(QObject *textEdit);

    QString formatName() const;
    void setFormatName(const QString &formatName);

    RepositoryWrapper *repository() const;
    void setRepository(RepositoryWrapper *repository);

Q_SIGNALS:
    void textEditChanged() const;
    void formatNameChanged() const;
    void repositoryChanged();

private:
    KSyntaxHighlighting::Repository *unwrappedRepository() const;

    QObject *m_textEdit;
    QString m_formatName;
    RepositoryWrapper *m_repository = nullptr;
    KSyntaxHighlighting::SyntaxHighlighter *m_highlighter = nullptr;
};

#endif // KQUICKSYNTAXHIGHLIGHTER_H
