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

#include <repository.h>
#include <definition.h>

#include <QFileInfo>
#include <QObject>
#include <QtTest/qtest.h>

using namespace SyntaxHighlighting;

class RepositoryTest : public QObject
{
    Q_OBJECT
private:
        Repository m_repo;

private Q_SLOTS:
    void testDefinitionByExtension_data()
    {
        QTest::addColumn<QString>("fileName");
        QTest::addColumn<QString>("defName");

        QTest::newRow("empty") << QString() << QString();
        QTest::newRow("qml") << QStringLiteral("/bla/foo.qml") << QStringLiteral("QML");
        QTest::newRow("glsl") << QStringLiteral("flat.frag") << QStringLiteral("GLSL");
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
            QVERIFY(def.load());
            QVERIFY(!def.name().isEmpty());
            QVERIFY(!def.translatedName().isEmpty());
            QVERIFY(!def.section().isEmpty());
            QVERIFY(!def.translatedSection().isEmpty());
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
};

QTEST_MAIN(RepositoryTest)

#include "syntaxrepository_test.moc"
