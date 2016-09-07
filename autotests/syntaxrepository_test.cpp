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
};

QTEST_MAIN(RepositoryTest)

#include "syntaxrepository_test.moc"
