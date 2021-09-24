Syntax Highlighting QML API   {#qml_api}
==================

-   [Basic Highlighting](#basic_highlighting)
-   [Advanced Use](#advanced_use)
-   [Further References](#refs)

<a name="basic_highlighting">

## Basic Highlighting

If all you need is doing syntax highlighting for a fixed language and using a color
theme matching the current system palette this is a matter of adding the `SyntaxHighlighter`
component from the `org.kde.syntaxhighlighting` QML module to a text control.

~~~
import QtQuick.Controls 2.15
import org.kde.syntaxhighlighting 1.0

TextArea {
    text: "..."
    SyntaxHighlighter {
        id: myHighlighter
        textEdit: parent
        definition: "C++"
    }
}
~~~

<a name="advanced_use">

## Advanced Use

For more complex uses the syntax definition might not be fixed but depend on input data (e.g.
derived from its mimetype or file name), or a user selection. In the C++ API this is enabled
by the `Repository` class, which is now also available in QML as a singleton object.

The following example shows how to use this for a simple syntax selection combo box.

~~~
ComboBox {
    model: Repository.definitions
    displayText: currentValue.translatedName + " (" + currentValue.translatedSection + ")"
    textRole: "translatedName"
    onCurrentIndexChanged: myHighlighter.definition = currentValue
}
~~~

Handling color themes is also possible, similarly to syntax definitions. Themes can be listed,
their properties can be accessed and they can be set by theme object or name on the highlighter.
Like in the C++ API it's also possible to just ask for the light or dark default theme.

<a name="refs">

## Further References

There's a more complete and pure-QML example in `examples/qml/example.qml` provided as part of
the KSyntaxHighlighting source code that can be run with `qmlscene`.
