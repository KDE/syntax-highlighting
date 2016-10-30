/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "codeeditor.h"

#include <definition.h>
#include <syntaxhighlighter.h>
#include <theme.h>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMenu>
#include <QPainter>
#include <QPalette>

class CodeEditorSidebar : public QWidget
{
    Q_OBJECT
public:
    explicit CodeEditorSidebar(CodeEditor *editor);
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    CodeEditor *m_codeEditor;
};

CodeEditorSidebar::CodeEditorSidebar(CodeEditor *editor) :
    QWidget(editor),
    m_codeEditor(editor)
{
}

QSize CodeEditorSidebar::sizeHint() const
{
    return QSize(m_codeEditor->sidebarWidth(), 0);
}

void CodeEditorSidebar::paintEvent(QPaintEvent *event)
{
    m_codeEditor->sidebarPaintEvent(event);
}


CodeEditor::CodeEditor(QWidget *parent) :
    QPlainTextEdit(parent),
    m_highlighter(new SyntaxHighlighting::SyntaxHighlighter(document())),
    m_sideBar(new CodeEditorSidebar(this))
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    setTheme((palette().color(QPalette::Base).lightness() < 128)
        ? m_repository.defaultTheme(SyntaxHighlighting::Repository::DarkTheme)
        : m_repository.defaultTheme(SyntaxHighlighting::Repository::LightTheme));

    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateSidebarGeometry);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateSidebarArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateSidebarGeometry();
    highlightCurrentLine();
}

CodeEditor::~CodeEditor()
{
}

void CodeEditor::openFile(const QString& fileName)
{
    QFile f(fileName);
    if (!f.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open" << fileName << ":" << f.errorString();
        return;
    }

    const auto def = m_repository.definitionForFileName(fileName);
    m_highlighter->setDefinition(def);

    setWindowTitle(fileName);
    setPlainText(QString::fromUtf8(f.readAll()));
}

void CodeEditor::contextMenuEvent(QContextMenuEvent *event)
{
    auto menu = createStandardContextMenu(event->pos());
    menu->addSeparator();
    auto openAction = menu->addAction(QStringLiteral("Open File..."));
    connect(openAction, &QAction::triggered, this, [this]() {
        const auto fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Open File"));
        if (!fileName.isEmpty())
            openFile(fileName);
    });

    // syntax selection
    auto hlActionGroup = new QActionGroup(menu);
    hlActionGroup->setExclusive(true);
    auto hlGroupMenu = menu->addMenu(QStringLiteral("Syntax"));
    auto noHlAction = hlGroupMenu->addAction(QStringLiteral("None"));
    noHlAction->setCheckable(true);
    hlActionGroup->addAction(noHlAction);
    noHlAction->setChecked(!m_highlighter->definition().isValid());
    QMenu *hlSubMenu = Q_NULLPTR;
    QString currentGroup;
    foreach (const auto &def, m_repository.definitions()) {
        if (def.isHidden())
            continue;
        if (currentGroup != def.section()) {
            currentGroup = def.section();
            hlSubMenu = hlGroupMenu->addMenu(def.translatedSection());
        }

        Q_ASSERT(hlSubMenu);
        auto action = hlSubMenu->addAction(def.translatedName());
        action->setCheckable(true);
        action->setData(def.name());
        hlActionGroup->addAction(action);
        if (def.name() == m_highlighter->definition().name())
            action->setChecked(true);
    }
    connect(hlActionGroup, &QActionGroup::triggered, this, [this](QAction *action) {
        const auto defName = action->data().toString();
        const auto def = m_repository.definitionForName(defName);
        m_highlighter->setDefinition(def);
    });

    // theme selection
    auto themeGroup = new QActionGroup(menu);
    themeGroup->setExclusive(true);
    auto themeMenu = menu->addMenu(QStringLiteral("Theme"));
    foreach (const auto &theme, m_repository.themes()) {
        auto action = themeMenu->addAction(theme.translatedName());
        action->setCheckable(true);
        action->setData(theme.name());
        themeGroup->addAction(action);
        if (theme.name() == m_highlighter->theme().name())
            action->setChecked(true);
    }
    connect(themeGroup, &QActionGroup::triggered, this, [this](QAction *action) {
        const auto themeName = action->data().toString();
        const auto theme = m_repository.theme(themeName);
        setTheme(theme);
    });

    menu->exec(event->globalPos());
    delete menu;
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    updateSidebarGeometry();
}

void CodeEditor::setTheme(const SyntaxHighlighting::Theme &theme)
{
    auto pal = qApp->palette();
    if (theme.isValid()) {
        pal.setColor(QPalette::Base, theme.editorColor(SyntaxHighlighting::Theme::BackgroundColor));
        pal.setColor(QPalette::Text, theme.textColor(SyntaxHighlighting::Theme::Normal));
        pal.setColor(QPalette::Highlight, theme.editorColor(SyntaxHighlighting::Theme::TextSelection));
    }
    setPalette(pal);

    m_highlighter->setTheme(theme);
    m_highlighter->rehighlight();
    highlightCurrentLine();
}

int CodeEditor::sidebarWidth() const
{
    int digits = 1;
    auto count = blockCount();
    while (count >= 10) {
        ++digits;
        count /= 10;
    }
    return 4 + fontMetrics().width(QLatin1Char('9')) * digits;
}

void CodeEditor::sidebarPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_sideBar);
    painter.fillRect(event->rect(), m_highlighter->theme().editorColor(SyntaxHighlighting::Theme::IconBorder));

    auto block = firstVisibleBlock();
    auto blockNumber = block.blockNumber();
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + blockBoundingRect(block).height();
    const int currentBlockNumber = textCursor().blockNumber();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const auto number = QString::number(blockNumber + 1);
            painter.setPen(m_highlighter->theme().editorColor(
                (blockNumber == currentBlockNumber) ? SyntaxHighlighting::Theme::CurrentLineNumber
                                                    : SyntaxHighlighting::Theme::LineNumbers));
            painter.drawText(0, top, m_sideBar->width() - 2, fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::updateSidebarGeometry()
{
    setViewportMargins(sidebarWidth(), 0, 0, 0);
    const auto r = contentsRect();
    m_sideBar->setGeometry(QRect(r.left(), r.top(), sidebarWidth(), r.height()));
}

void CodeEditor::updateSidebarArea(const QRect& rect, int dy)
{
    if (dy)
        m_sideBar->scroll(0, dy);
    else
        m_sideBar->update(0, rect.y(), m_sideBar->width(), rect.height());
}

void CodeEditor::highlightCurrentLine()
{
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(m_highlighter->theme().editorColor(SyntaxHighlighting::Theme::CurrentLine)));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(selection);
    setExtraSelections(extraSelections);
}

#include "codeeditor.moc"
