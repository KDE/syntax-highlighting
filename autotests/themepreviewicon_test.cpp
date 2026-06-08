/*
    SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>

    SPDX-License-Identifier: MIT
*/

#include "test-config.h"

#include <KSyntaxHighlighting/Theme>

#include <QObject>
#include <QStandardPaths>
#include <QTest>

namespace KSyntaxHighlighting
{

class ThemePreviewIconTest : public QObject
{
    Q_OBJECT
private:
    Repository m_repo;

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
        initRepositorySearchPaths(m_repo);
    }

    void testPreviewIconInvalid()
    {
        QVERIFY(Theme().previewIcon().isNull());
    }

    void testPreviewIcon_data()
    {
        QTest::addColumn<QString>("themeName");
        QTest::addColumn<QSize>("iconSize");

        QTest::newRow("Breeze Light 16x16") << QStringLiteral("Breeze Light") << QSize(16, 16);
        QTest::newRow("Breeze Light 32x16") << QStringLiteral("Breeze Light") << QSize(32, 16);
        QTest::newRow("Breeze Light 32x32") << QStringLiteral("Breeze Light") << QSize(32, 32);

        QTest::newRow("Breeze Dark 16x16") << QStringLiteral("Breeze Dark") << QSize(16, 16);
        QTest::newRow("Breeze Dark 32x16") << QStringLiteral("Breeze Dark") << QSize(32, 16);
        QTest::newRow("Breeze Dark 32x32") << QStringLiteral("Breeze Dark") << QSize(32, 32);
    }

    void testPreviewIcon()
    {
        QFETCH(QString, themeName);
        QFETCH(QSize, iconSize);

        Theme theme = m_repo.theme(themeName);
        QVERIFY(theme.isValid());

        QIcon previewIcon = theme.previewIcon();
        QVERIFY(!previewIcon.isNull());

        QPixmap pixmap = previewIcon.pixmap(iconSize, 1.0 /*dpr*/);
        QVERIFY(!pixmap.isNull());
        QImage image = pixmap.toImage().convertedTo(QImage::Format_ARGB32);

        const QString referenceFileName = QStringLiteral("themepreview_%1_%2x%3.png").arg(themeName).arg(iconSize.width()).arg(iconSize.height());
        QPixmap reference(QFINDTESTDATA(QStringLiteral("themepreview/%1").arg(referenceFileName)));
        QVERIFY(!reference.isNull());
        QImage referenceImage = reference.toImage().convertedTo(QImage::Format_ARGB32);

        // Converting to QImage first so we can ensure the formats are identical.
        // QPixmap optimizes pixmaps without alpha channel to an image format without.
        QCOMPARE(image, referenceImage);
    }
};
}

QTEST_MAIN(KSyntaxHighlighting::ThemePreviewIconTest)

#include "themepreviewicon_test.moc"
