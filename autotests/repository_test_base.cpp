/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2021 Igor Kushnir <igorkuo@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "repository_test_base.h"

#include "test-config.h"

#include <KSyntaxHighlighting/Definition>

#include <QStandardPaths>
#include <QTest>

namespace
{
constexpr const QChar definitionSeparator = QLatin1Char{'\n'};
struct DefinitionDataRow {
    const char *dataTag;

    const char *m_inputString;
    const char *m_definitionName;

    QString inputString() const
    {
        return QString::fromUtf8(m_inputString);
    }
    QString firstDefinitionName() const
    {
        return QString::fromUtf8(m_definitionName).section(definitionSeparator, 0, 0);
    }
    QStringList definitionNames() const
    {
        return m_definitionName[0] == 0 ? QStringList{} : QString::fromUtf8(m_definitionName).split(definitionSeparator);
    }
};

// The two tables below have been copied to ktexteditor:autotests/src/katemodemanager_test_base.cpp and adjusted.
// The two versions of the tables should be kept in sync.

constexpr DefinitionDataRow definitionsForFileNames[] = {
    {"empty", "", ""},

    {"*.tar.gz", "noMatch.tar.gz", ""},
    {"No match", "a_random#filename", ""},
    {"Long path, no match", "/this/is/a/moderately/long/path/to/no-match", ""},
    {"Prefix in dir name", "Kconfig/no_match", ""},

    {"*.qml", "/bla/foo.qml", "QML"},
    {"*.frag", "flat.frag", "GLSL"},
    {"*.md", "highPriority.md", "Markdown"},
    {"*.octave", "lowPriority.octave", "Octave"},
    {"*.hats", "sameLastLetterPattern.hats", "ATS"},

    {"*.c", "test.c", "C\nANSI C89\nLPC"},
    {"*.fs", "test.fs", "FSharp\nANS-Forth94\nGLSL"},
    {"*.m", "/bla/foo.m", "Objective-C\nMagma\nMatlab\nOctave"},

    {"Makefile", "Makefile", "Makefile"},
    {"Path to Makefile", "/some/path/to/Makefile", "Makefile"},
    {"Makefile.*", "Makefile.am", "Makefile"},

    {"not-Makefile.dic", "not-Makefile.dic", "Hunspell Dictionary File"},
    {"*qmakefile.cpp", "test_qmakefile.cpp", "C++\nISO C++\nGCCExtensions\nSystemC"},
    {"*_makefile.mm", "bench_makefile.mm", "Objective-C++\nMetamath"},

    {"xorg.conf", "/etc/literal-pattern/xorg.conf", "x.org Configuration\nTOML"},
    {".profile", "2-literal-patterns/.profile", "Bash\nZsh"},

    {"Config.*", "Config.beginning", "Kconfig"},
    {"usr.libexec.*", "usr.libexec.", "AppArmor Security Profile"},
    {"Jam*", "Jam-beginning-no-dot", "Jam"},
    {"usr.li-*.ch", "usr.li-many-partial-prefix-matches.ch", "xHarbour\nClipper"},
    {"QRPG*.*", "QRPG1u4[+.unusual", "ILERPG"},

    {"*patch", "no-dot-before-ending~patch", "Diff"},
    {"*.cmake.in", "two-dots-after-asterisk.cmake.in", "CMake"},
    {"*.html.mst", "two-dots-priority!=0.html.mst", "Mustache/Handlebars (HTML)"},

    {"*.desktop.cmake", "2_suffixes.desktop.cmake", ".desktop\nCMake"},
    {"*.per.err", "2_suffixes-but-one-a-better-match.per.err", "4GL\n4GL-PER"},
    {"*.xml.eex", "2_suffixes-one-lang.xml.eex", "Elixir"},
    {"fishd.*.fish", "fishd.prefix,suffix=one-lang.fish", "Fish"},

    {"usr.bin.*.ftl", "usr.bin.heterogenousPatternMatch.ftl", "AppArmor Security Profile\nFTL\nFluent"},
    {"Doxyfile.*.pro", "Doxyfile.heterogenous.Pattern-Match.pro", "QMake\nDoxyfile\nProlog\nRSI IDL"},
    {"Kconfig*.ml", "KconfigHeterogenous_pattern_match.ml", "Objective Caml\nSML\nKconfig"},
    {"snap-confine.*.html.rac", "snap-confine.2.-higher-priority.html.rac", "Mustache/Handlebars (HTML)\nAppArmor Security Profile"},
    {"file_contexts_*.fq.gz", "file_contexts_prefix-higher-priority.fq.gz", "SELinux File Contexts\nFASTQ"},
    {"QRPG*.ninja", "QRPG.ninja", "ILERPG\nNinja"},
    {"qrpg*.tt", "qrpgTwoUnusualPatterns.tt", "ILERPG\nTT2"},
    {"qrpg*.cl", "qrpg$heterogenous~pattern&match.cl", "OpenCL\nCommon Lisp\nILERPG"},
    {".gitignore*.tt*.textile", ".gitignoreHeterogenous3.tt.textile", "Textile\nGit Ignore\nTT2"},
};

constexpr DefinitionDataRow definitionsForMimeTypeNames[] = {
    {"empty", "", ""},

    {"Nonexistent MIME type", "text/nonexistent-mt", ""},
    {"No match", "application/x-bzip-compressed-tar", ""},

    {"High priority", "text/rust", "Rust"},
    {"Negative priority", "text/octave", "Octave"},

    {"Multiple types match", "text/x-chdr", "C++\nISO C++\nC\nGCCExtensions\nANSI C89\nSystemC"},
};

template<std::size_t size>
void addFirstDefinitionDataRows(const DefinitionDataRow (&array)[size])
{
    for (const auto &row : array) {
        QTest::newRow(row.dataTag) << row.inputString() << row.firstDefinitionName();
    }
}
template<std::size_t size>
void addDefinitionsDataRows(const DefinitionDataRow (&array)[size])
{
    for (const auto &row : array) {
        QTest::newRow(row.dataTag) << row.inputString() << row.definitionNames();
    }
}

void verifyDefinition(const KSyntaxHighlighting::Definition &definition, const QString &definitionName)
{
    if (definitionName.isEmpty()) {
        QVERIFY(!definition.isValid());
    } else {
        QVERIFY(definition.isValid());
        QCOMPARE(definition.name(), definitionName);
    }
}

void verifyDefinitionList(const QList<KSyntaxHighlighting::Definition> &definitionList, const QStringList &definitionNames)
{
    QStringList names;
    for (const auto &definition : definitionList) {
        names.push_back(definition.name());
    }
    QCOMPARE(names, definitionNames);
}
} // unnamed namespace

void RepositoryTestBase::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
    initRepositorySearchPaths(m_repo);
}

void RepositoryTestBase::definitionByExtensionTestData()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("definitionName");
    addFirstDefinitionDataRows(definitionsForFileNames);
}

void RepositoryTestBase::definitionByExtensionTest(const QString &fileName, const QString &definitionName)
{
    verifyDefinition(m_repo.definitionForFileName(fileName), definitionName);
}

void RepositoryTestBase::definitionsForFileNameTestData()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QStringList>("definitionNames");
    addDefinitionsDataRows(definitionsForFileNames);
}

void RepositoryTestBase::definitionsForFileNameTest(const QString &fileName, const QStringList &definitionNames)
{
    verifyDefinitionList(m_repo.definitionsForFileName(fileName), definitionNames);
}

void RepositoryTestBase::definitionForMimeTypeTestData()
{
    QTest::addColumn<QString>("mimeTypeName");
    QTest::addColumn<QString>("definitionName");
    addFirstDefinitionDataRows(definitionsForMimeTypeNames);
}

void RepositoryTestBase::definitionForMimeTypeTest(const QString &mimeTypeName, const QString &definitionName)
{
    verifyDefinition(m_repo.definitionForMimeType(mimeTypeName), definitionName);
}

void RepositoryTestBase::definitionsForMimeTypeTestData()
{
    QTest::addColumn<QString>("mimeTypeName");
    QTest::addColumn<QStringList>("definitionNames");
    addDefinitionsDataRows(definitionsForMimeTypeNames);
}

void RepositoryTestBase::definitionsForMimeTypeTest(const QString &mimeTypeName, const QStringList &definitionNames)
{
    verifyDefinitionList(m_repo.definitionsForMimeType(mimeTypeName), definitionNames);
}

#include "moc_repository_test_base.cpp"
