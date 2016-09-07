#!/bin/sh
$EXTRACT_TR_STRINGS `find . -name \*.cpp -o -name \*.h -o -name \*.ui -o -name \*.qml` -o $podir/syntaxhighlighting.pot
