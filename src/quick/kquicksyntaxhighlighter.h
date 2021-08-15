/*
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KQUICKSYNTAXHIGHLIGHTER_H
#define KQUICKSYNTAXHIGHLIGHTER_H

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

public:
    explicit KQuickSyntaxHighlighter(QObject *parent = nullptr);
    ~KQuickSyntaxHighlighter() override;

    QObject *textEdit() const;
    void setTextEdit(QObject *textEdit);

    QString formatName() const;
    void setFormatName(const QString &formatName);

Q_SIGNALS:
    void textEditChanged() const;
    void formatNameChanged() const;

private:
    static int m_instanceCount;
    QObject *m_textEdit;
    QString m_formatName;
    static KSyntaxHighlighting::Repository *m_repository;
    KSyntaxHighlighting::SyntaxHighlighter *m_highlighter;
};

#endif // KQUICKSYNTAXHIGHLIGHTER_H
