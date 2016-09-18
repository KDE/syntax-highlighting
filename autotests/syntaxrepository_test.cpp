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

#include <abstracthighlighter.h>
#include <definition.h>
#include <repository.h>
#include <state.h>

#include <QFileInfo>
#include <QObject>
#include <QStandardPaths>
#include <QtTest/qtest.h>

namespace SyntaxHighlighting {

class NullHighlighter : public AbstractHighlighter
{
public:
    using AbstractHighlighter::highlightLine;
    void setFormat(int offset, int length, const Format &format) Q_DECL_OVERRIDE
    {
        Q_UNUSED(offset);
        Q_UNUSED(length);
        Q_UNUSED(format);
    }
};

class RepositoryTest : public QObject
{
    Q_OBJECT
private:
        Repository m_repo;

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::enableTestMode(true);
    }

    void testDefinitionByExtension_data()
    {
        QTest::addColumn<QString>("fileName");
        QTest::addColumn<QString>("defName");

        QTest::newRow("empty") << QString() << QString();
        QTest::newRow("qml") << QStringLiteral("/bla/foo.qml") << QStringLiteral("QML");
        QTest::newRow("glsl") << QStringLiteral("flat.frag") << QStringLiteral("GLSL");
        // the following ones are defined in multiple syntax definitions
        QTest::newRow("c") << QStringLiteral("test.c") << QStringLiteral("C");
        QTest::newRow("c++") << QStringLiteral("test.cpp") << QStringLiteral("C++");
        QTest::newRow("markdown") << QStringLiteral("test.md") << QStringLiteral("Markdown");
    }

    void testDefinitionByExtension()
    {
        QFETCH(QString, fileName);
        QFETCH(QString, defName);

        auto def = m_repo.definitionForFileName(fileName);
        if (defName.isEmpty()) {
            QVERIFY(!def.isValid());
        } else {
            QVERIFY(def.isValid());
            QCOMPARE(def.name(), defName);
        }
    }

    void testLoadAll()
    {
        foreach (auto def, m_repo.definitions()) {
            QVERIFY(!def.name().isEmpty());
            QVERIFY(!def.translatedName().isEmpty());
            QVERIFY(!def.section().isEmpty());
            QVERIFY(!def.translatedSection().isEmpty());
            // indirectly trigger loading, as we can't reach that from public API
            // if the loading fails the highlighter will produce empty states
            NullHighlighter hl;
            State initialState;
            hl.setDefinition(def);
            if (def.name() == QLatin1String("Kconfig")) // FIXME this ends in an infinite loop!
                continue;
            const auto state = hl.highlightLine(QLatin1String("This should not crash } ] ) !"), initialState);
            QVERIFY(state != initialState);
        }
    }

    void testMetaData()
    {
        auto def = m_repo.definitionForName(QLatin1String("Alerts"));
        QVERIFY(def.isValid());
        QVERIFY(def.extensions().isEmpty());
        QVERIFY(def.mimeTypes().isEmpty());
        QVERIFY(def.version() >= 1.11f);
        QVERIFY(def.isHidden());
        QCOMPARE(def.section(), QLatin1String("Other"));
        QCOMPARE(def.license(), QLatin1String("LGPL"));
        QVERIFY(def.author().contains(QLatin1String("Dominik")));
        QFileInfo fi(def.filePath());
        QVERIFY(fi.isAbsolute());
        QVERIFY(def.filePath().endsWith(QLatin1String("alert.xml")));

        def = m_repo.definitionForName(QLatin1String("C++"));
        QVERIFY(def.isValid());
        QCOMPARE(def.section(), QLatin1String("Sources"));
        QCOMPARE(def.indenter(), QLatin1String("cstyle"));
        QCOMPARE(def.style(), QLatin1String("C++"));
        QVERIFY(def.mimeTypes().contains(QLatin1String("text/x-c++hdr")));
        QVERIFY(def.extensions().contains(QLatin1String("*.h")));
        QCOMPARE(def.priority(), 9);

        def = m_repo.definitionForName(QLatin1String("Apache Configuration"));
        QVERIFY(def.isValid());
        QVERIFY(def.extensions().contains(QLatin1String("httpd.conf")));
        QVERIFY(def.extensions().contains(QLatin1String(".htaccess*")));
    }

    void testReload()
    {
        auto def = m_repo.definitionForName(QLatin1String("QML"));
        QVERIFY(!m_repo.definitions().isEmpty());
        QVERIFY(def.isValid());

        NullHighlighter hl;
        hl.setDefinition(def);
        auto oldState = hl.highlightLine(QLatin1String("/* TODO this should not crash */"), State());

        m_repo.reload();
        QVERIFY(!m_repo.definitions().isEmpty());
        QVERIFY(!def.isValid());

        hl.highlightLine(QLatin1String("/* TODO this should not crash */"), State());
        hl.highlightLine(QLatin1String("/* FIXME neither should this crash */"), oldState);
        QVERIFY(hl.definition().isValid());
        QCOMPARE(hl.definition().name(), QLatin1String("QML"));
    }
};
}

QTEST_MAIN(SyntaxHighlighting::RepositoryTest)

#include "syntaxrepository_test.moc"
