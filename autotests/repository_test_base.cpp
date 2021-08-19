/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2021 Igor Kushnir <igorkuo@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "repository_test_base.h"

#include "test-config.h"

#include <definition.h>
#include <repository.h>

#include <QStandardPaths>
#include <QTest>

namespace
{
constexpr const QChar definitionSeparator = QLatin1Char{'\n'};
struct DefinitionDataRow {
    const char *dataTag;

    const char *m_fileName;
    const char *m_definitionName;

    QString fileName() const
    {
        return QString::fromUtf8(m_fileName);
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
// This table has been copied to ktexteditor:autotests/src/katemodemanager_test_base.cpp and adjusted.
// The two versions of the table should be kept in sync.
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

    {"xorg.conf", "/etc/literal-pattern/xorg.conf", "x.org Configuration"},
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

    for (const auto &row : definitionsForFileNames) {
        QTest::newRow(row.dataTag) << row.fileName() << row.firstDefinitionName();
    }
}

void RepositoryTestBase::definitionByExtensionTest(const QString &fileName, const QString &definitionName)
{
    const auto definition = m_repo.definitionForFileName(fileName);
    if (definitionName.isEmpty()) {
        QVERIFY(!definition.isValid());
    } else {
        QVERIFY(definition.isValid());
        QCOMPARE(definition.name(), definitionName);
    }
}

void RepositoryTestBase::definitionsForFileNameTestData()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QStringList>("definitionNames");

    for (const auto &row : definitionsForFileNames) {
        QTest::newRow(row.dataTag) << row.fileName() << row.definitionNames();
    }
}

void RepositoryTestBase::definitionsForFileNameTest(const QString &fileName, const QStringList &definitionNames)
{
    const auto definitions = m_repo.definitionsForFileName(fileName);
    QStringList names;
    for (const auto &definition : definitions) {
        names.push_back(definition.name());
    }
    QCOMPARE(names, definitionNames);
}
