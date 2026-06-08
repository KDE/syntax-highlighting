/*
 *  SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 *  SPDX-License-Identifier: MIT
 */

#include "themepreviewicon_p.h"

#include <QIconEngine>
#include <QPainter>

using namespace KSyntaxHighlighting;

ThemePreviewIconEngine::ThemePreviewIconEngine(const KSyntaxHighlighting::Theme &theme)
    : QIconEngine()
    , m_theme(theme)
{
}

QIconEngine *ThemePreviewIconEngine::clone() const
{
    return new ThemePreviewIconEngine(*this);
}

QSize ThemePreviewIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode);
    Q_UNUSED(state);
    const int squareSize = std::min(size.width(), size.height());
    return QSize(squareSize, squareSize);
}

QPixmap ThemePreviewIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    return scaledPixmap(size, mode, state, 1.0 /*scale*/);
}

QPixmap ThemePreviewIconEngine::scaledPixmap(const QSize &size, QIcon::Mode mode, QIcon::State state, qreal scale)
{
    QPixmap pixmap(size * scale);
    pixmap.setDevicePixelRatio(scale);
    const bool isSquare = size.width() == size.height();
    if (isSquare) {
        pixmap.fill(Qt::black);
    } else {
        pixmap.fill(Qt::transparent);
    }

    QPainter p(&pixmap);
    // skip frame, when we already filled the pixmap black.
    paint(&p, QRect(QPoint(0, 0), size), mode, state, !isSquare);

    return pixmap;
}

void ThemePreviewIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    paint(painter, rect, mode, state, true /*includeFrame*/);
}

void ThemePreviewIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state, bool includeFrame)
{
    QRect adjustedRect(QPoint(0, 0), actualSize(rect.size(), mode, state));
    adjustedRect.moveCenter(rect.center());

    if (includeFrame) {
        painter->fillRect(adjustedRect, Qt::black);
    }

    painter->save();
    painter->translate(adjustedRect.topLeft());
    // Draw a little color scheme preview,
    // inspired by KColorSchemeManagerPrivate::createPreview.
    const int margin = std::max(1, adjustedRect.width() / 32);
    const int itemSize = adjustedRect.width() / 2 - margin;

    painter->fillRect(margin, margin, itemSize, itemSize, QColor::fromRgba(m_theme.textColor(KSyntaxHighlighting::Theme::TextStyle::Normal)));
    painter->fillRect(margin + itemSize,
                      margin,
                      itemSize,
                      itemSize,
                      QColor::fromRgba(m_theme.editorColor(KSyntaxHighlighting::Theme::EditorColorRole::BackgroundColor)));
    painter->fillRect(margin, margin + itemSize, itemSize, itemSize, QColor::fromRgba(m_theme.textColor(KSyntaxHighlighting::Theme::TextStyle::Keyword)));
    painter->fillRect(margin + itemSize,
                      margin + itemSize,
                      itemSize,
                      itemSize,
                      QColor::fromRgba(m_theme.editorColor(KSyntaxHighlighting::Theme::EditorColorRole::TextSelection)));

    painter->restore();
}
