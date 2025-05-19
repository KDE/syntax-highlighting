0!d
0,3!d
0,+3
0,~3
0,/a/
144s/hello/world/
2{N;s/\n//;}
/a/,$
/a/,/b/
//d
$d
/d/d;;d

/./{H;$!d} ; x ; s/^/\nSTART-->/ ; s/$/\n<--END/

# invalid separator
\éfdfde

:x /\\$/ { N; s/\\\n//g ; bx }

:a ; $!N ; s/\n\s+/ / ; ta ; P ; D

$!N;s/\n  */ /;ta P;D

1 {
    x
    s/^$/          /
    s/^.*$/&&&&&&&&/
    x
}

y/<TAB>/ /
s/^\(.\{81\}\).*$/\1/

/[^0-9]/ d
/\//! s/^/.\//

s/^\(.*\/\)\(.*\)\n\(.*\)$/mv "\1\2" "\1\3"/p

/^{/ q

{N; /\b(\w+)\s+\1\b/{=;p} ; D}

s/(.{40,40})/\1\n/

by

/a/! s/[b-h]*/&0/

/[a-h]/ b loop

# text
adsds\
dsds
a\
sa\\ns\na

s/\n[^fd]\w\B+*s\x13s\`\po/d\1a\\b&c\U\ps/g0mwfile
s/\x1_\x11_\x111__\d1_\d11_\d111_\d1111__\o1_\o11_\o111_\o1111//

y~a\
~bc~;g

l3
l 3
ecmd
