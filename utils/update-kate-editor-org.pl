#!/usr/bin/perl -w

# update script for kate-editor.org/syntax
# SPDX-FileCopyrightText: 2020 Christoph Cullmann <cullmann@kde.org>
# SPDX-License-Identifier: MIT

# needed things
use FindBin;
use Cwd;
use File::Basename;
use File::Copy;
use File::Path qw(make_path remove_tree);
use XML::Parser;

# be strict & warn
use strict;
use warnings;

# we need as parameter both the source and the build directory
my $sourceDir = shift;
my $buildDir = shift;
if (!defined($sourceDir) || !defined($buildDir)) {
    die "Not all arguments provided, valid call: update-kate-editor-org.pl <source directory> <build directory>\n";
}

# output settings
print "Using source directory: $sourceDir\n";
print "Using build directory: $buildDir\n";

# switch to build directory, we do all our work there
chdir($buildDir) || die "Failed to switch to build directory '$buildDir'!\n";

# get kate-editor.org clone from invent.kde.org, update if already around
if (-d "kate-editor-org") {
    print "Updating kate-editor.org clone...\n";
    system("git", "-C", "kate-editor-org", "pull") == 0 || die "Failed to pull kate-editor-org.git!\n";
} else {
    print "Creating kate-editor.org clone...\n";
    system("git", "clone", "git\@invent.kde.org:websites/kate-editor-org.git") == 0 || die "Failed to clone kate-editor-org.git!\n";
}

#
# update of syntax definitions
# beside the pure update site generation, we will create some web site with examples for all highlightings
#

# try to get current frameworks version
my $currentVersion;
open (my $list, "<$sourceDir/CMakeLists.txt");
for (<$list>) {
    if ((my $version) = /^set\(KF_VERSION "6.([0-9]+)\.[0-9]+"\)/) {
       $currentVersion = $version;
       last;
    }
}
close $list;
if (!defined($currentVersion)) {
    die "Failed to determine current version of syntax-highlighting framework!\n"
}

# current maximal version
print "Current version of syntax-highlighting: 6.$currentVersion\n";

# purge old data in kate-editor.org clone
my $staticSyntaxPath = "kate-editor-org/static/syntax";
my $staticSyntaxPathData = "$staticSyntaxPath/data/syntax";
remove_tree($staticSyntaxPath);
if (-d $staticSyntaxPath) {
    die "Failed to delete '$staticSyntaxPath'!\n";
}
make_path($staticSyntaxPathData);
if (! -d $staticSyntaxPathData) {
    die "Failed to create '$staticSyntaxPathData'!\n";
}

# collect all known syntax files from the generated resource file and copy them over
open my $resourceFile, "<$buildDir/data/syntax-data-xml.qrc";
while (<$resourceFile>) {
    if ((my $file) = /<file>(.*\.xml)<\/file>/) {
        copy($file, $staticSyntaxPathData) or die "Copy failed: $!";
    }
}
close $resourceFile;

# copy over all html references as examples
system("cp", "-rf", "$sourceDir/autotests/html", "$staticSyntaxPath/data/html") == 0 || die "Failed to copy HTML references!\n";

# switch to kate-editor.org syntax directory now for post-processing
chdir($staticSyntaxPath) || die "Failed to switch to '$staticSyntaxPath' directory!\n";

# add new data to kate-editor.org git
system("git", "add", "data") == 0 || die "Failed to add syntax files to git!\n";

# setup XML parser with handler for start element
my %languageAttributes = ();
sub start_tag
{
    # we only care for the language element, remember the attributes
    my($p, $tag, %attrs) = @_;
    if ($tag eq "language") {
        %languageAttributes = %attrs;
        $p->finish();
    }
}
my $parser = XML::Parser->new( Handlers => { Start => \&start_tag });

# read all syntax files and remember for their version infos!
print "Parsing XML syntax/*.xml files...\n";
my %metaInfo;
my %nameToFile;
my $count = 0;
foreach my $xmlFile (<data/syntax/*.xml>) {
    # parse the file
    %languageAttributes = ();
    $parser->parsefile( $xmlFile );

    # we need a name!
    my $name = $languageAttributes{'name'};
    if (!defined($name)) {
        print "Skipping $xmlFile as name attribute is missing.\n";
        next;
    }

    # if we have no versions set, we can't handle this file!
    my $version = $languageAttributes{'version'};
    if (!defined($version)) {
        print "Skipping $xmlFile as version attribute is missing.\n";
        next;
    }
    my $kateversion = $languageAttributes{'kateversion'};
    if (!defined($kateversion)) {
        print "Skipping $xmlFile as kateversion attribute is missing.\n";
        next;
    }

    # remember attributes
    # print "Remembering $xmlFile '$name' with version=$version & kateversion=$kateversion\n";
    foreach my $key (keys %languageAttributes) {
        $metaInfo{$xmlFile}{$key} = $languageAttributes{$key};
    }

    # remember section => name => file mapping
    $nameToFile{$languageAttributes{'section'}}{$name} = $xmlFile;
    ++$count;
}

# now: generate all needed update-*.xml files
print "Generating XML update-*.xml files...\n";
for (my $majorVersion = 5; $majorVersion <= 6; ++$majorVersion) {
    my $minorVersion = 0;
    while ($minorVersion <= (($majorVersion == 6) ? $currentVersion : 256)) {
        # generate one update file
        my $cVersion = "$majorVersion.$minorVersion";
        #print "Generation update-$cVersion.xml...\n";
        open (my $update, ">update-$cVersion.xml");
        print $update "<!DOCTYPE DEFINITIONS>\n";
        print $update "<DEFINITIONS>\n";
        foreach my $def (sort keys %metaInfo) {
            # is this definition allowed here?
            $_ = $metaInfo{$def}{kateversion};
            if ((my $major, my $minor) = /([0-9]+)\.([0-9]+)/) {
                next if (($major > $majorVersion) || ($major == $majorVersion && $minor > $minorVersion));
            } else {
                next;
            }
            print $update "<Definition name=\"$metaInfo{$def}{name}\" url=\"https://kate-editor.org/syntax/$def\" version=\"$metaInfo{$def}{version}\"/>\n";
        }
        print $update "</DEFINITIONS>\n";
        close $update;

        # add to git
        system("git add update-$cVersion.xml") == 0 || die "Failed to add update-$cVersion.xml to git!\n";

        # next one
        ++$minorVersion;
    }
}

# parse the html files to match them to the highlighting they belong to
# we just search for the dark variants and derive the names for the non-dark from that
print "Parsing HTML example syntax/data/html/*.html files...\n";
my %nameToHTML;
foreach my $htmlFile (<data/html/*.dark.html>, <data/html/.*.dark.html>) {
    my $name;
    open my $F, "<$htmlFile";
    while (<$F>) {
        if (($name) = /name="generator" content="KF5::SyntaxHighlighting - Definition \((.*)\) - Theme/) {
            last;
        }
    }
    close $F;
    if (defined($name)) {
        $htmlFile =~ s/\.dark\.html//;
        if (defined($nameToHTML{$name})) {
            die "Duplicated test output found for '$name' (".$nameToHTML{$name}." vs. ".$htmlFile.").\ntesthighlighter_test in the framework should not have allowed that!\n";
        }
        $nameToHTML{$name} = $htmlFile;
    } else {
        print "Skipping $htmlFile as proper generator meta information tag missing.\n";
    }
}

# for better l10n, only generate a YAML file containing syntax data
# a Hugo template will be combined with the data to generate the overview page /syntax
print "Generating syntax data in data/syntax.yaml...\n";
make_path("../../data");
my $syntax_path = "../../data/syntax.yaml";
open (my $syntax_handle, ">$syntax_path");
print $syntax_handle "# This file is auto-generated by \"make update_kate_editor_org\" in syntax-highlighting.git\n";
foreach my $section (sort keys %nameToFile) {
    foreach my $name (sort keys %{$nameToFile{$section}}) {
        my $file = $nameToFile{$section}{$name};
        print $syntax_handle
            "- name: $name\n".
            "  section: $section\n".
            "  file: /syntax/$file\n";

        # link example output if existing
        if (defined($nameToHTML{$name})) {
            print $syntax_handle "  examples: [/syntax/".$nameToHTML{$name}.".html, /syntax/".$nameToHTML{$name}.".dark.html]\n";
        } else {
            print $syntax_handle "  examples: []\n";
        }
    }
}
close($syntax_handle);

# add to git
system("git add $syntax_path") == 0 || die "Failed to add $syntax_path to git!\n";

#
# update of themes web site
# this will generate an overview of all shipped themes with an example
#

# switch back to build directory, we do all our work there
chdir($buildDir) || die "Failed to switch to build directory '$buildDir'!\n";

# purge old data in kate-editor.org clone
my $staticThemePath = "kate-editor-org/static/themes";
remove_tree($staticThemePath);
if (-d $staticThemePath) {
    die "Failed to delete '$staticThemePath'!\n";
}
make_path($staticThemePath);
if (! -d $staticThemePath) {
    die "Failed to create '$staticThemePath'!\n";
}

# copy over all html renderings as examples
print "Updating theme example HTML files...\n";
system("cp", "-rf", "autotests/theme.html.output", "$staticThemePath/html") == 0 || die "Failed to copy autotests/theme.html.output references!\n";

# switch over to git again
chdir($staticThemePath) || die "Failed to switch to '$staticThemePath' directory!\n";

# collect all themes with their test case
print "Parsing theme kate-editor-org/static/themes/html/*.html files...\n";
my %themeToHTML;
foreach my $htmlFile (<html/*.html>) {
    my $name;
    open my $F, "<$htmlFile";
    while (<$F>) {
        if (($name) = /name="generator" content="KF5::SyntaxHighlighting - Definition \(.*\) - Theme \((.*)\)"/) {
            last;
        }
    }
    close $F;
    if (defined($name)) {
        $themeToHTML{$name} = $htmlFile;
    } else {
        print "Skipping $htmlFile as proper generator meta information tag missing.\n";
    }
}

# create HTML snippets one can embed into a page from the theme HTML pages
# we will hash the stuff from lower case name to real name + file for better output order below
my %themeToHTMLSnippet;
foreach my $name (sort keys %themeToHTML) {
    # get full file
    open my $F, "<$themeToHTML{$name}";
    my $fullFile = do { local $/; <$F> };
    close $F;

    # kill <body ...><pre> start and replace it with simple <pre> with body attribute
    $fullFile =~ s@.*<body style="(.*)"><pre>@<pre style="$1">@s;

    # kill ending </pre>... and replace it with simple </pre>
    $fullFile =~ s@</pre></body></html>@</pre>@g;

    # write snippet to disk
    my $snippetName = $themeToHTML{$name};
    $snippetName =~ s/\.html/-snippet.html/;
    open my $OF, ">$snippetName";
    print $OF $fullFile;
    close $OF;
    $themeToHTMLSnippet{lc($name)}{"name"} = $name;
    $themeToHTMLSnippet{lc($name)}{"file"} = $snippetName;
}

# add html files
system("git", "add", "html") == 0 || die "Failed to add theme HTML files to git!\n";

# for better l10n, only generate a YAML file containing theme data
# a Hugo template will be combined with the data to generate the overview page /themes
# we output sorted by lower case names as otherwise ayu and co. end up at the end...
print "Generating theme data in data/themes.yaml...\n";
my $themes_path = "../../data/themes.yaml";
open (my $themes_handle, ">$themes_path");
print $themes_handle "# This file is auto-generated by \"make update_kate_editor_org\" in syntax-highlighting.git\n";
foreach my $lcName (sort keys %themeToHTMLSnippet) {
    my $name = $themeToHTMLSnippet{$lcName}{"name"};
    my $file = $themeToHTMLSnippet{$lcName}{"file"};
    print $themes_handle
        "- name: $name\n".
        "  file: /static/themes/$file\n";
}
close($themes_handle);

# add to git
system("git add $themes_path") == 0 || die "Failed to add $themes_path to git!\n";
