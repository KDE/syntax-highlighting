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
