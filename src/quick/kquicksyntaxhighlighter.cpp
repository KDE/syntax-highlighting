/*
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "kquicksyntaxhighlighter.h"

#include <definition.h>
#include <repository.h>
#include <syntaxhighlighter.h>
#include <theme.h>

#include <QGuiApplication>
#include <QPalette>
#include <QQuickTextDocument>
#include <QTextDocument>

using namespace KSyntaxHighlighting;

extern Repository *defaultRepository();

KQuickSyntaxHighlighter::KQuickSyntaxHighlighter(QObject *parent)
    : QObject(parent)
    , m_textEdit(nullptr)
    , m_highlighter(new KSyntaxHighlighting::SyntaxHighlighter(this))
{
}

KQuickSyntaxHighlighter::~KQuickSyntaxHighlighter() = default;

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

        m_highlighter->setTheme(unwrappedRepository()->themeForPalette(QGuiApplication::palette()));
        m_highlighter->setDefinition(unwrappedRepository()->definitionForName(m_formatName));
    }
}

RepositoryWrapper *KQuickSyntaxHighlighter::repository() const
{
    return m_repository;
}

void KQuickSyntaxHighlighter::setRepository(RepositoryWrapper *repository)
{
    if (m_repository == repository) {
        return;
    }
    m_repository = repository;
    Q_EMIT repositoryChanged();
}

Repository *KQuickSyntaxHighlighter::unwrappedRepository() const
{
    if (m_repository) {
        return m_repository->m_repository;
    }
    return defaultRepository();
}
