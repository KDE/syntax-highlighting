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
    if ((my $version) = /^set\(KF_VERSION "[0-9]+\.([0-9]+)\.[0-9]+"\)/) {
       $currentVersion = $version;
       last;
    }
}
close $list;
if (!defined($currentVersion)) {
    die "Failed to determine current version of syntax-highlighting framework!\n"
}

# current maximal version
print "Current version of syntax-highlighting: 5.$currentVersion\n";

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
open my $resourceFile, "<$buildDir/data/syntax-data.qrc";
while (<$resourceFile>) {
    if ((my $file) = />(.*\.xml)<\/file>/) {
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
my $minorVersion = 0;
while ($minorVersion <= $currentVersion) {
    # generate one update file
    my $cVersion = "5.$minorVersion";
    #print "Generation update-$cVersion.xml...\n";
    open (my $update, ">update-$cVersion.xml");
    print $update "<!DOCTYPE DEFINITIONS>\n";
    print $update "<DEFINITIONS>\n";
    foreach my $def (sort keys %metaInfo) {
        # is this definition allowed here?
        $_ = $metaInfo{$def}{kateversion};
        if ((my $version) = /[0-9]+\.([0-9]+)/) {
           next if ($version > $minorVersion);
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

# we create some overview page, too, as markdown to brag about the amount of stuff we know .P
print "Generating syntax.md overview page...\n";
my $syntax_md = "../../content/syntax.md";
open (my $syntax_page, ">$syntax_md");
print $syntax_page
    "---\n".
    "title: Syntax Highlighting\n".
    "hideMeta: true\n".
    "author: Christoph Cullmann\n".
    "date: 2019-08-24T12:12:12+00:00\n".
    "menu:\n".
    "  main:\n".
    "    weight: 110\n".
    "    parent: menu\n".
    "---\n\n".
    "<!-- This page is auto-generated by \"make update_kate_editor_org\" in syntax-highlighting.git -->\n\n".
    "<p>Kate's highlighting is powered by the <a href=\"https://api.kde.org/frameworks/syntax-highlighting/html/\">KSyntaxHighlighting</a> framework and supports $count different languages.\n\n</p>".
    "<p>Below you can find a list of all supported languages together with example renderings of the highlighting both as light and dark theme HTML pages.\n\n</p>".
    "<p>Submissions of new language definitions & examples are very welcome, head over to the <a href=\"https://invent.kde.org/frameworks/syntax-highlighting#introduction\">KSyntaxHighlighting repository README</a> for details.\n\n</p>".
    "<table><tr><th>Category</th><th>Language</th><th colspan=2 nowrap>Light & Dark Theme Examples</th></tr>\n";
foreach my $section (sort keys %nameToFile) {
    foreach my $name (sort keys %{$nameToFile{$section}}) {
        my $file = $nameToFile{$section}{$name};
        print $syntax_page "<tr><td>$section</td><td><a target=_blank href=\"/syntax/$file\">$name</a></td>\n";

        # link example output if existing
        if (defined($nameToHTML{$name})) {
            print $syntax_page "<td><a target=_blank href=\"/syntax/".$nameToHTML{$name}.".html\">Light Theme</a></td>";
            print $syntax_page "<td><a target=_blank href=\"/syntax/".$nameToHTML{$name}.".dark.html\">Dark Theme</a></td>";
        } else {
            print $syntax_page "<td colspan=2><center><a target=_blank href=\"https://invent.kde.org/frameworks/syntax-highlighting#adding-unit-tests-for-a-syntax-definition\">Submit Example</a></center></td>\n";
        }
        print $syntax_page "</tr>\n";
    }
}
print $syntax_page "</table>\n";
close($syntax_page);

# add to git
system("git add $syntax_md") == 0 || die "Failed to add $syntax_md to git!\n";

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

# we create some overview page, too, as markdown to brag about the amount of stuff we know .P
# we output sorted by lower case names as otherwise ayu and co. end up at the end...
print "Generating themes.md overview page...\n";
my $themes_md = "../../content/themes.md";
open (my $themes_page, ">$themes_md");
print $themes_page
    "---\n".
    "title: Color Themes\n".
    "hideMeta: true\n".
    "author: Christoph Cullmann\n".
    "date: 2019-08-24T12:12:12+00:00\n".
    "menu:\n".
    "  main:\n".
    "    weight: 120\n".
    "    parent: menu\n".
    "---\n\n".
    "<!-- This page is auto-generated by \"make update_kate_editor_org\" in syntax-highlighting.git -->\n\n".
    "<p>Kate's highlighting is powered by the <a href=\"https://api.kde.org/frameworks/syntax-highlighting/html/\">KSyntaxHighlighting</a> framework that contains ".scalar(keys %themeToHTML)." bundled color themes.\n".
    "Below you can find a list of all bundled themes together with an example rendering.\n\n</p>";
foreach my $lcName (sort keys %themeToHTMLSnippet) {
    my $name = $themeToHTMLSnippet{$lcName}{"name"};
    my $file = $themeToHTMLSnippet{$lcName}{"file"};
    print $themes_page "<h3>$name</h3>\n";
    print $themes_page "{{< rawhtml >}}\n";
    print $themes_page "{{% include \"/static/themes/$file\" %}}\n";
    print $themes_page "{{< /rawhtml >}}\n\n";
}
close($themes_page);

# add to git
system("git add $themes_md") == 0 || die "Failed to add $themes_md to git!\n";
