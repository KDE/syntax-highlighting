#!/bin/bash

find "$@" -name '*.h' -o -name '*.cpp' -o -name '*.qml' -o -name '*.c' | grep -v /3rdparty/ | grep -v /build | while read FILE; do
    if grep -qiE "Copyright \(C\) [0-9, -]{4,} " "$FILE" ; then continue; fi
    thisfile=`basename $FILE`
    authorName=`git config user.name`
    authorEmail=`git config user.email`
    thisYear=`date +%Y`
    cat <<EOF > "$FILE".tmp
/*
    Copyright (C) $thisYear $authorName <$authorEmail>

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

EOF
    cat "$FILE" >> "$FILE".tmp
    mv "$FILE".tmp "$FILE"
done

