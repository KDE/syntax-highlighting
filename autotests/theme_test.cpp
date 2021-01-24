/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "test-config.h"

#include <abstracthighlighter.h>
#include <definition.h>
#include <format.h>
#include <htmlhighlighter.h>
#include <repository.h>
#include <state.h>
#include <theme.h>

#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QObject>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTest>

namespace KSyntaxHighlighting
{
class FormatCollector : public AbstractHighlighter
{
public:
    using AbstractHighlighter::highlightLine;
    void applyFormat(int offset, int length, const Format &format) Q_DECL_OVERRIDE
    {
        Q_UNUSED(offset);
        Q_UNUSED(length);
        formatMap.insert(format.name(), format);
    }
    QHash<QString, Format> formatMap;
};

class ThemeTest : public QObject
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

    void testThemes()
    {
        QVERIFY(!m_repo.themes().isEmpty());
        for (const auto &theme : m_repo.themes()) {
            QVERIFY(theme.isValid());
            QVERIFY(!theme.name().isEmpty());
            QVERIFY(!theme.filePath().isEmpty());
            QVERIFY(QFileInfo::exists(theme.filePath()));
            QVERIFY(m_repo.theme(theme.name()).isValid());
        }
    }

    void testFormat_data()
    {
        QTest::addColumn<QString>("themeName");
        QTest::newRow("default") << "Breeze Light";
        QTest::newRow("dark") << "Breeze Dark";
        QTest::newRow("print") << "Printing";
    }

    void testFormat()
    {
        QFETCH(QString, themeName);

        // somewhat complicated way to get proper Format objects
        FormatCollector collector;
        collector.setDefinition(m_repo.definitionForName(QLatin1String("QML")));
        const auto t = m_repo.theme(themeName);
        QVERIFY(t.isValid());
        collector.setTheme(t);
        collector.highlightLine(QLatin1String("normal + property real foo: 3.14"), State());

        QVERIFY(collector.formatMap.size() >= 4);
        // qDebug() << collector.formatMap.keys();

        // normal text
        auto f = collector.formatMap.value(QLatin1String("Normal Text"));
        QVERIFY(f.isValid());
        QVERIFY(f.textStyle() == Theme::Normal);
        QVERIFY(f.isDefaultTextStyle(t));
        QVERIFY(!f.hasTextColor(t));
        QVERIFY(!f.hasBackgroundColor(t));
        QVERIFY(f.id() > 0);

        // visually identical to normal text with Printing theme
        f = collector.formatMap.value(QLatin1String("Symbol"));
        QVERIFY(f.isValid());
        QCOMPARE(f.textStyle(), Theme::Operator);
        if (themeName == QLatin1String("Printing")) {
            QVERIFY(f.isDefaultTextStyle(t));
            QVERIFY(!f.hasTextColor(t));
        } else {
            QVERIFY(!f.isDefaultTextStyle(t));
            QVERIFY(f.hasTextColor(t));
        }
        QVERIFY(f.id() > 0);

        // visually different to normal text
        f = collector.formatMap.value(QLatin1String("Keywords"));
        QVERIFY(f.isValid());
        QCOMPARE(f.textStyle(), Theme::Keyword);
        QVERIFY(!f.isDefaultTextStyle(t));
        QVERIFY(f.isBold(t));
        QVERIFY(f.id() > 0);

        f = collector.formatMap.value(QLatin1String("Float"));
        QVERIFY(f.isValid());
        QCOMPARE(f.textStyle(), Theme::Float);
        QVERIFY(!f.isDefaultTextStyle(t));
        QVERIFY(f.hasTextColor(t));
        QVERIFY(f.id() > 0);
    }

    void testBreezeLightTheme()
    {
        Theme t = m_repo.theme(QLatin1String("Breeze Light"));
        QVERIFY(t.isValid());

        // Themes compiled in as resource are never writable
        QVERIFY(t.isReadOnly());

        // make sure all editor colors are properly read
        QCOMPARE(t.editorColor(Theme::BackgroundColor), QColor("#ffffff").rgba());
        QCOMPARE(t.editorColor(Theme::TextSelection), QColor("#94caef").rgba());
        QCOMPARE(t.editorColor(Theme::CurrentLine), QColor("#f8f7f6").rgba());
        QCOMPARE(t.editorColor(Theme::SearchHighlight), QColor("#ffff00").rgba());
        QCOMPARE(t.editorColor(Theme::ReplaceHighlight), QColor("#00ff00").rgba());
        QCOMPARE(t.editorColor(Theme::BracketMatching), QColor("#ffff00").rgba());
        QCOMPARE(t.editorColor(Theme::TabMarker), QColor("#d2d2d2").rgba());
        QCOMPARE(t.editorColor(Theme::SpellChecking), QColor("#bf0303").rgba());
        QCOMPARE(t.editorColor(Theme::IndentationLine), QColor("#d2d2d2").rgba());
        QCOMPARE(t.editorColor(Theme::IconBorder), QColor("#f0f0f0").rgba());
        QCOMPARE(t.editorColor(Theme::CodeFolding), QColor("#94caef").rgba());
        QCOMPARE(t.editorColor(Theme::LineNumbers), QColor("#a0a0a0").rgba());
        QCOMPARE(t.editorColor(Theme::CurrentLineNumber), QColor("#1e1e1e").rgba());
        QCOMPARE(t.editorColor(Theme::WordWrapMarker), QColor("#ededed").rgba());
        QCOMPARE(t.editorColor(Theme::ModifiedLines), QColor("#fdbc4b").rgba());
        QCOMPARE(t.editorColor(Theme::SavedLines), QColor("#2ecc71").rgba());
        QCOMPARE(t.editorColor(Theme::Separator), QColor("#d5d5d5").rgba());
        QCOMPARE(t.editorColor(Theme::MarkBookmark), QColor("#0000ff").rgba());
        QCOMPARE(t.editorColor(Theme::MarkBreakpointActive), QColor("#ff0000").rgba());
        QCOMPARE(t.editorColor(Theme::MarkBreakpointReached), QColor("#ffff00").rgba());
        QCOMPARE(t.editorColor(Theme::MarkBreakpointDisabled), QColor("#ff00ff").rgba());
        QCOMPARE(t.editorColor(Theme::MarkExecution), QColor("#a0a0a4").rgba());
        QCOMPARE(t.editorColor(Theme::MarkWarning), QColor("#00ff00").rgba());
        QCOMPARE(t.editorColor(Theme::MarkError), QColor("#ff0000").rgba());
        QCOMPARE(t.editorColor(Theme::TemplateBackground), QColor("#d6d2d0").rgba());
        QCOMPARE(t.editorColor(Theme::TemplatePlaceholder), QColor("#baf8ce").rgba());
        QCOMPARE(t.editorColor(Theme::TemplateFocusedPlaceholder), QColor("#76da98").rgba());
        QCOMPARE(t.editorColor(Theme::TemplateReadOnlyPlaceholder), QColor("#f6e6e6").rgba());
    }

    void testInvalidTheme()
    {
        // somewhat complicated way to get proper Format objects
        FormatCollector collector;
        collector.setDefinition(m_repo.definitionForName(QLatin1String("QML")));
        collector.highlightLine(QLatin1String("normal + property real foo: 3.14"), State());

        QVERIFY(collector.formatMap.size() >= 4);
        auto f = collector.formatMap.value(QLatin1String("Normal Text"));
        QVERIFY(f.isValid());
        QVERIFY(f.isDefaultTextStyle(Theme()));
        QVERIFY(!f.hasTextColor(Theme()));
        QVERIFY(!f.hasBackgroundColor(Theme()));
    }

    void testThemeIntegrity_data()
    {
        // cleanup before we test
        QDir(QStringLiteral(TESTBUILDDIR "/theme.html.output/")).removeRecursively();
        QDir().mkpath(QStringLiteral(TESTBUILDDIR "/theme.html.output/"));

        QTest::addColumn<QString>("themeFileName");

        QDirIterator it(QStringLiteral(":/org.kde.syntax-highlighting/themes"), QStringList() << QLatin1String("*.theme"), QDir::Files);
        while (it.hasNext()) {
            const QString fileName = it.next();
            QTest::newRow(fileName.toLatin1().data()) << fileName;
        }
    }

    static bool isColorEntry(const QString &entry)
    {
        static const QLatin1String predefColorEntries[] = {QLatin1String("text-color"),
                                                           QLatin1String("selected-text-color"),
                                                           QLatin1String("background-color"),
                                                           QLatin1String("selected-background-color")};

        return std::find(std::begin(predefColorEntries), std::end(predefColorEntries), entry) != std::end(predefColorEntries);
    }

    static bool isFontStyleEntry(const QString &entry)
    {
        static const QLatin1String predefColorEntries[] = {QLatin1String("bold"),
                                                           QLatin1String("italic"),
                                                           QLatin1String("underline"),
                                                           QLatin1String("strike-through")};

        return std::find(std::begin(predefColorEntries), std::end(predefColorEntries), entry) != std::end(predefColorEntries);
    }

    void verifyStyle(const QJsonObject &textStyle, const QString &textStyleName, QVector<QString> &unknown)
    {
        const QStringList definedColors = textStyle.keys();
        for (const auto &key : definedColors) {
            const QString context = textStyleName + QLatin1Char('/') + key + QLatin1Char('=') + textStyle.value(key).toString();
            if (isColorEntry(key)) {
                QVERIFY2(QColor::isValidColor(textStyle.value(key).toString()), context.toLatin1().data());
            } else if (isFontStyleEntry(key)) {
                QVERIFY2(textStyle.value(key).isBool(), context.toLatin1().data());
            } else {
                unknown.append(textStyleName + QLatin1Char('/') + key);
            }
        }
    }

    void testThemeIntegrity()
    {
        QFETCH(QString, themeFileName);

        QFile loadFile(themeFileName);
        QVERIFY(loadFile.open(QIODevice::ReadOnly));
        const QByteArray jsonData = loadFile.readAll();
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "Failed to parse theme file:" << parseError.errorString();
            QVERIFY(false);
        }

        QJsonObject obj = jsonDoc.object();

        // verify metadata
        QVERIFY(obj.contains(QLatin1String("metadata")));
        const QJsonObject metadata = obj.value(QLatin1String("metadata")).toObject();
        QVERIFY(metadata.contains(QLatin1String("name")));
        const auto themeName = metadata.value(QLatin1String("name")).toString();
        QVERIFY(!themeName.isEmpty());
        QVERIFY(metadata.contains(QLatin1String("revision")));
        QVERIFY(metadata.value(QLatin1String("revision")).toInt() > 0);

        // verify licensing part of the metadata

        // ensure we have some copyright text attributes like "SPDX-FileCopyrightText: 2020 Christoph Cullmann <cullmann@kde.org>"
        QVERIFY(metadata.contains(QLatin1String("copyright")));
        const auto copyrights = metadata.value(QLatin1String("copyright")).toArray();
        QVERIFY(!copyrights.empty());
        for (const auto &copyright : copyrights) {
            static const QRegularExpression copyrightRegex(QLatin1String("SPDX-FileCopyrightText: \\d{4} "));
            QVERIFY(copyright.toString().indexOf(copyrightRegex) == 0);
        }

        // ensure the theme is MIT licensed with a proper SPDX identifier
        // we always compile all themes into the library as resources, we want to have a "pure" MIT licensed library!
        QVERIFY(metadata.contains(QLatin1String("license")));
        QVERIFY(metadata.value(QLatin1String("license")).toString() == QLatin1String("SPDX-License-Identifier: MIT"));

        // verify completeness of text styles
        static const auto idx = Theme::staticMetaObject.indexOfEnumerator("TextStyle");
        QVERIFY(idx >= 0);
        const auto metaEnum = Theme::staticMetaObject.enumerator(idx);
        QVERIFY(obj.contains(QLatin1String("text-styles")));
        const QJsonObject textStyles = obj.value(QLatin1String("text-styles")).toObject();
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            QCOMPARE(i, metaEnum.value(i));
            const QString textStyleName = QLatin1String(metaEnum.key(i));
            QVERIFY(textStyles.contains(textStyleName));
            const QJsonObject textStyle = textStyles.value(textStyleName).toObject();
            QVERIFY(textStyle.contains(QLatin1String("text-color")));

            // verify valid entry
            QVector<QString> unknown;
            verifyStyle(textStyle, textStyleName, unknown);
            if (!unknown.isEmpty()) {
                qWarning() << "Unknown entries found in text-styles: " << unknown;
            }
            QVERIFY(unknown.isEmpty());
        }

        // editor area colors
        static const auto colorIdx = Theme::staticMetaObject.indexOfEnumerator("EditorColorRole");
        Q_ASSERT(colorIdx >= 0);
        const auto metaEnumColor = Theme::staticMetaObject.enumerator(colorIdx);
        QStringList requiredEditorColors;
        for (int i = 0; i < metaEnumColor.keyCount(); ++i) {
            Q_ASSERT(i == metaEnumColor.value(i));
            requiredEditorColors.append(QLatin1String(metaEnumColor.key(i)));
        }
        std::sort(requiredEditorColors.begin(), requiredEditorColors.end());

        // verify all editor colors are defined - not more, not less
        QVERIFY(obj.contains(QLatin1String("editor-colors")));
        const QJsonObject editorColors = obj.value(QLatin1String("editor-colors")).toObject();
        QStringList definedEditorColors = editorColors.keys();
        std::sort(definedEditorColors.begin(), definedEditorColors.end());
        QCOMPARE(definedEditorColors, requiredEditorColors);

        // verify all editor colors are valid
        for (const auto &key : requiredEditorColors) {
            QVERIFY(QColor::isValidColor(editorColors.value(key).toString()));
        }

        // verify custom-styles if any
        {
            QVector<QPair<QString, QString>> invalidCustomStyles;
            const auto customStyles = obj.value(QLatin1String("custom-styles")).toObject();
            for (auto it = customStyles.constBegin(); it != customStyles.constEnd(); ++it) {
                // get definitions for this language
                const auto &lang = it.key();
                const auto def = m_repo.definitionForName(lang);
                QVERIFY2(def.isValid(), qPrintable(QStringLiteral("Definition %1 does not exist").arg(lang)));

                const QVector<Format> fmts = def.formats();
                QSet<QString> fmtNames;
                fmtNames.reserve(fmts.size());
                for (const auto &fmt : fmts) {
                    fmtNames.insert(fmt.name());
                }

                // get custom style names for `lang` in this theme
                // and make sure the language definition contain that name
                const auto customStylesForLang = it.value().toObject();
                for (auto csIt = customStylesForLang.constBegin(); csIt != customStylesForLang.constEnd(); ++csIt) {
                    // make sure the text style is present in language definition formats
                    const auto &textStyleName = csIt.key();

                    // wasn't found, append it to the vector
                    // we will later print this and fail the test
                    if (!fmtNames.contains(textStyleName)) {
                        invalidCustomStyles.append({lang, textStyleName});
                        continue;
                    }

                    // now verify this text style
                    const auto entry = csIt.value().toObject();
                    QVector<QString> unknown;
                    verifyStyle(entry, textStyleName, unknown);
                    if (!unknown.isEmpty()) {
                        qWarning() << "Unknown entries found in custom-styles for " << lang << ": " << unknown;
                    }
                    QVERIFY(unknown.isEmpty());
                }
            }

            if (!invalidCustomStyles.isEmpty()) {
                qWarning() << "Unknown styles found: " << invalidCustomStyles;
            }
            QVERIFY(invalidCustomStyles.isEmpty());
        }

        // the theme must be available in our repository, too
        const auto theme = m_repo.theme(themeName);
        QVERIFY(theme.isValid());
        QVERIFY(theme.name() == themeName);

        // we have one fixed theme showcase
        const QString inFile(QStringLiteral(TESTSRCDIR "/input/themes/showcase.cpp"));

        // render some example HTML for the theme, we use that e.g. to show-case the themes on our website
        const QString outFile(QStringLiteral(TESTBUILDDIR "/theme.html.output/") + QFileInfo(theme.filePath()).baseName() + QStringLiteral(".html"));
        HtmlHighlighter highlighter;
        highlighter.setTheme(theme);
        QVERIFY(highlighter.theme().isValid());
        highlighter.setDefinition(m_repo.definitionForFileName(inFile));
        highlighter.setOutputFile(outFile);
        highlighter.highlightFile(inFile);
    }
};
}

QTEST_GUILESS_MAIN(KSyntaxHighlighting::ThemeTest)

#include "theme_test.moc"
