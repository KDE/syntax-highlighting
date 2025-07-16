#!/usr/bin/perl -w
# This is a pseudo Perl file to test Kate's Perl syntax highlighting.
# TODO: this is incomplete, add more syntax examples!

sub prg($)
{
	my $var = shift;

	$var =~ s/bla/foo/igs;
	$var =~ s!bla!foo!igs;
	$var =~ s#bla#foo#igs;
	$var =~ s#(.)#foo$1foo#igs;
	$var =~ tr/a-z/A-Z/;
	($match) = ($var =~ m/(.*?)/igs);

	$test = 2/453453.21;
	$test /= 2;

	print qq~d fsd fsdf sdfl sd~
	
	$" = '/';
	
	$foo = <<__EOF;
d ahfdklf klsdfl sdf sd
fsd sdf sdfsdlkf sd
__EOF

	$x = "dasds";

	next if( $match eq "two" );
	next if( $match =~ /go/i );

	@array = (1,2,3);		# a comment
	# Test qw versions with special ending characters
	@array = qw(apple foo bar);
	@array = qw[apple foo bar];
	@array = qw{apple foo bar};
	@array = qw<apple foo bar>;
	@array = qw(
		multi
		line
		test
	);
	# Test qw with non special ending characters;
	@array = qw/apple foo bar/;
	@array = qw|apple foo bar|;
	@array = qw@apple foo bar@;
	@array = qw!apple foo bar!;
	@array = qw"apple foo bar";
	@array = qw'apple foo bar';
	push(@array, 4);
	%hash = (red => 'rot',
		blue => 'blau');
	print keys(%hash);
}

sub blah {
    my $str = << '    EOS';
        this is my string
        and it's continuation
    EOS

    $str = "hello world";

    $str = << "    EOS";
        this is my string
        and it's continuation
    EOS
}

&blah;
prg("test");

# Bracket closures in RegExp patterns (bug #364866)
qr{ ${var} aa{aa{a}a} aa*b?};
qr(aa(a(a(a(b|c)a)a)a)aa*b?);
s{aaa {aaa} a \x{A2} *b?}{aa};
s(aa(a(a(a(b|c)a)a)a)aa)(aa);

# Bracket closures in RegExp replacement
s(abc) # TODO bla bla
    (aa{(b)}c)g;
s(abc) # TODO bla bla
    [aa{(b)}c]g;
s(abc) # TODO bla bla
    {aa{(b)}c}g;
s[abc] # TODO bla bla
    (aa{(b)}c);
s{(abc)} # TODO bla bla
    [aa{(b)}c]g;
# any char for replacement
s{abc} # TODO bla bla
    +aa{(b)}c+g;
s{abc} # TODO bla bla
    ]aa{(b)}c]g; # this is fine

# Strings as scalar references (bug #348765)
$x = \'Reference of a String';
$y = \"Reference of a String";

# Variables that start with underscore (bug #355300)
$_variable
$_ # Reserved var.

for my $x ($hash->{arr}->@*) {
    for my $k (keys $k->%*) {
        ...
    }
}

# Highlight correctly operator // (bug #407327)
$x = ns // "";
print $x;

# Number
print _1__2_3 _0x1__2_3 _0b1 _0123 # not a Number
print 123 1__2__3__ 000
print 0x123 0x__1__2__3__
print 0b10 0b__1__0__
print 0123 0__1__2__3__
print 123.123 123. 123.__ 123.__1
print 123e123 123.e__2 123.__e__2
print 012.23 123e__ # no float
print 1__2__3__.1__2__3__e__1__
print 1__2__3__.1__2__3__e__ # no float
print 1__2__3__e__1__
print 1__2__3__e__ # no float
print 0x1p8 0x123__p1__0 0x1p8__
print 0x1p_8 # no float
