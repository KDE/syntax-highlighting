#!/bin/sh

# extract language and section names from XML syntax definition files
# and adapt for lupdate-based extraction to match the rest of the code
$EXTRACTATTR --attr=language,name,Language --attr="language,section,Language Section" ../data/*.xml >> rc.cpp || exit 12
sed -i -e 's/^i18nc/QT_TRANSLATE_NOOP/' rc.cpp

grep --no-filename '"name"' ../data/themes/*.theme > rc_themes.cpp
sed -i -e 's/"name"/QT_TRANSLATE_NOOP("Theme", /' rc_themes.cpp
sed -i -e 's/://' rc_themes.cpp
sed -i -e 's/,$/)/' rc_themes.cpp

$EXTRACT_TR_STRINGS `find . -name \*.cpp -o -name \*.h -o -name \*.ui -o -name \*.qml` -o $podir/syntaxhighlighting5_qt.pot

rm rc_themes.cpp
