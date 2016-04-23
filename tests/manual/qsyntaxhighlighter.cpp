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

#include <syntaxhighlighter.h>
#include <syntaxrepository.h>

#include <QApplication>
#include <QFile>
#include <QTextEdit>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if (argc <= 1)
        return 1;

    QFile f(app.arguments().last());
    if (!f.open(QFile::ReadOnly))
        return 1;

    KateSyntax::SyntaxRepository repo;

    QTextEdit edit;
    edit.show();
    auto hl = new KateSyntax::SyntaxHighlighter(&edit);
    hl->setDefinition(repo.definitionForFileName(f.fileName()));

    edit.setPlainText(QString::fromUtf8(f.readAll()));
    hl->setDocument(edit.document());

    return app.exec();
}
