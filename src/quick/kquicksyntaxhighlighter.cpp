/*
    SPDX-FileCopyrightText: 2018 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2021 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "kquicksyntaxhighlighter.h"

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

QVariant KQuickSyntaxHighlighter::definition() const
{
    return QVariant::fromValue(m_definition);
}

void KQuickSyntaxHighlighter::setDefinition(const QVariant &definition)
{
    Definition def;
    if (definition.type() == QVariant::String) {
        def = unwrappedRepository()->definitionForName(definition.toString());
    } else {
        def = definition.value<Definition>();
    }

    if (m_definition != def) {
        m_definition = def;

        m_highlighter->setTheme(unwrappedRepository()->themeForPalette(QGuiApplication::palette()));
        m_highlighter->setDefinition(def);

        Q_EMIT definitionChanged();
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
