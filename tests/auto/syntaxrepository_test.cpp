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

#include <syntaxrepository.h>
#include <syntaxdefinition.h>

#include <QObject>
#include <QtTest/qtest.h>

using namespace KateSyntax;

class SyntaxRepositoryTest : public QObject
{
    Q_OBJECT
private:
        SyntaxRepository m_repo;

private slots:
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
        foreach (auto def, m_repo.definitions())
            QVERIFY(def.load());
    }

};

QTEST_MAIN(SyntaxRepositoryTest)

#include "syntaxrepository_test.moc"
