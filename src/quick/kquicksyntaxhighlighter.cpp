/*
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "kquicksyntaxhighlighter.h"

#include <definition.h>
#include <repository.h>
#include <syntaxhighlighter.h>
#include <theme.h>

#include <QQuickTextDocument>
#include <QTextDocument>

int KQuickSyntaxHighlighter::m_instanceCount = 0;
KSyntaxHighlighting::Repository *KQuickSyntaxHighlighter::m_repository = nullptr;

KQuickSyntaxHighlighter::KQuickSyntaxHighlighter(QObject *parent)
    : QObject(parent)
    , m_textEdit(nullptr)
    , m_highlighter(new KSyntaxHighlighting::SyntaxHighlighter(this))
{
    ++m_instanceCount;
}

KQuickSyntaxHighlighter::~KQuickSyntaxHighlighter()
{
    --m_instanceCount;

    if (!m_instanceCount) {
        delete m_repository;
        m_repository = nullptr;
    }
}

QObject *KQuickSyntaxHighlighter::textEdit() const
{
    return m_textEdit;
}

void KQuickSyntaxHighlighter::setTextEdit(QObject *textEdit)
{
    if (m_textEdit != textEdit) {
        m_textEdit = textEdit;
        m_highlighter->setDocument(m_textEdit->property("textDocument").value<QQuickTextDocument *>()->textDocument());
    }
}

QString KQuickSyntaxHighlighter::formatName() const
{
    return m_formatName;
}

void KQuickSyntaxHighlighter::setFormatName(const QString &formatName)
{
    if (m_formatName != formatName) {
        m_formatName = formatName;

        if (!m_repository) {
            m_repository = new KSyntaxHighlighting::Repository();
        }

        m_highlighter->setTheme(m_repository->defaultTheme(KSyntaxHighlighting::Repository::LightTheme));

        m_highlighter->setDefinition(m_repository->definitionForName(m_formatName));
    }
}
