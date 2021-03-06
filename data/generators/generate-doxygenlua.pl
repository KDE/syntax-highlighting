#!/usr/bin/env perl
# SPDX-FileCopyrightText: 2020 Jonathan Poelen <jonathan.poelen@gmail.com>
# SPDX-License-Identifier: MIT

my $file = "";

open(my $input, '<:encoding(UTF-8)', $ARGV[0])
  or die "Could not open file '$ARGV[0]': $!";

open(my $output, '>:encoding(UTF-8)', $ARGV[1])
  or die "Could not open file '$ARGV[1]': $!";

while (<$input>)
{
  $file .= $_;
}

$warning = "\n\n<!-- ***** THIS FILE WAS GENERATED BY A SCRIPT - DO NOT EDIT ***** -->\n";
$first_context = "      <context attribute=\"Normal Text\" lineEndContext=\"#stay\" name=\"Normal\">
        <RegExpr attribute=\"Comment\" context=\"LineComment\" String=\"--(?:!|(?:-(?=[^-]|\$)))\"/>
        <RegExpr attribute=\"Region\" context=\"#stay\" String=\"--\\s*\@\\{\\s*\$\" beginRegion=\"MemberGroup\" />
        <RegExpr attribute=\"Region\" context=\"#stay\" String=\"--\\s*\@\\}\\s*\$\"   endRegion=\"MemberGroup\" />
      </context>";

$file =~ s/\n\s*<context [^\n]*?(?:name="ML_|name="BlockComment").*?<\/context>//gs;
$file =~ s/\n\s*<context [^\n]*?name="Normal".*?<\/context>/\n$first_context/s;
$file =~ s/\n[^\n]*?(?: ml_word|LineContinue)[^\n]+//gs;
$file =~ s/\/\/\//---/gs;
$file =~ s/\/\/!/--!/gs;

$language = $file =~ s/.*?(<language[^>]+?>).*/$1/sr;
$language =~ s/ name="[^"]+/ name="DoxygenLua/s;
$language =~ s/ extensions="[^"]+/ extensions="/s;
$language =~ s/ mimetype="[^"]+/ mimetype="/s;
$language =~ s/ priority="[^"]+"//s;
$version = $language =~ s/.*? version="([^"]+).*/$1/sr;
$version = $version+2;
$language =~ s/ version="[^"]+/ version="$version/s;
$file =~ s/<language[^>]+?>/$warning\n$language/s;

print $output $file;
print $output $warning;
