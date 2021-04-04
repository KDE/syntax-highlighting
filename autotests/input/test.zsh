# basic types:
echo 'single quoted string'
echo "double quoted string"
echo $'string with esc\apes\x0din it'
echo $"string meant to be translated"


# comments:
# this is a comment
#this too
echo this is#nt a comment
dcop kate EditInterface#1 #this is
grep -e "^default/linux/amd64/" |\ #this is not a comment but #this is
mkdir this\ isnt\ #a\ comment
mkdir this\ isnt\\\;#a\ comment
mkdir this\\ #is a comment

# brace expansion
mv my_file.{JPG,jpg}
echo f.{01..100..3} f.{#..Z} f.{\{..\}} f.{$i..$j..$p}
echo f.{01..100} f.{a..Z} f.{'a'..$Z}
# no brace expansion
echo f.{..100} f.{a..Z..}


# special characters are escaped:
echo \(output\) \&\| \> \< \" \' \*


# variable substitution:
echo $filename.ext
echo $filename_ext
echo ${filename}_ext
echo text${array[$subscript]}.text
echo text${array["string"]}.text
echo short are $_, $$, $?, ${@}, etc.
echo ${variable/a/d}
echo ${1:-default}
echo ${10} $10a
echo $! $=!


# expression subst:
echo $(( cd << ed + 1 ))


# command subst:
echo $(ls -l)
echo `cat myfile`


# file subst:
echo $(<$filename)
echo $(</path/to/myfile)

# process subst:
sort <(show_labels) | sed 's/a/bg' > my_file.txt 2>&1


# All substitutions also work in strings:
echo "subst ${in}side string"  'not $inside this ofcourse'
echo "The result is $(( $a + $b )). Thanks!"
echo "Your homedir contains `ls $HOME |wc -l` files."


# Escapes in strings:
p="String \` with \$ escapes \" ";


# keywords are black, builtins dark purple and common commands lighter purple
set
exit
login


# Other colorings:
error() {
	cat /usr/bin/lesspipe.sh
	runscript >& redir.bak
	exec 3>&4
}


# do - done make code blocks
while [ $p -lt $q ]
do
	chown 0644 $file.$p
done


# braces as well
run_prog | sort -u |
{
	echo Header
	while read a b d
	do
		echo $a/$b/$c
	done
	echo Footer
}


# Any constructions can be nested:
echo "A long string with $(
	if [ $count -gt 100 ] ; then
		echo "much"
	else
		echo "not much"
	fi ) substitutions." ;


# Even the case construct is correctly folded:
test -f blaat &&
(	do_something
	case $p in
		*bak)
			do_bak $p
			;;
		*)
			dont_bak $p
			;;
	esac
) # despite the extra parentheses in the case construction.


# variable assignments:
DIR=/dev
p=`ls`
LC_ALL="nl" dcop 'kate*'
_VAR=val
ARR=(this is an array)
ARR2=([this]=too [and]="this too")
usage="$0 -- version $VERSION
Multiple lines of output
can be possible."
ANSWER=yes	# here 'yes' isn't highlighed as command


# Some commands expect variable names, these are colored correctly:
export PATH=/my/bin:$PATH BLAAT
export A B D
local p=3  x  y='\'
read x y z <<< $hallo
unset B
declare -a VAR1 VAR2 && exit
declare less a && b
declare a=(1 2)
getopts :h:l::d arg
read #comment
let a=4+4 3+a b=c+3 d+3 d*4 # * is a glob

# options are recoqnized:
zip -f=file.zip
./configure  --destdir=/usr
make  destdir=/usr/


# [[ and [ correctly need spaces to be regarded as structure,
# otherwise they are patterns (currently treated as normal text)
if [ "$p" == "" ] ; then
	ls /usr/bin/[a-z]*
elif [[ $p == 0 ]] ; then
	ls /usr/share/$p
fi

# Fixed:
ls a[ab]*		# dont try to interprete as assignment with subscript (fixed)
a[ab]
a[ab]=sa


# Here documents are difficult to catch:
cat > myfile << __EOF__
You're right, this is definitely no bash code
But ls more $parameters should be expanded.
__EOF__


# quoted:
cat << "EOF" | egrep "this" >&4   # the rest of the line is still considered bash source
You're right, this is definitely no bash code
But ls more $parameters should be expanded. :->
EOF

cat <<bla || exit
bla bla
bla


# indented:
if true
then
	cat <<- EOF
		Indented text with a $dollar or \$two
	EOF
elif [ -d $file ]; then
	cat <<- "EOF"
		Indented text without a $dollar
	EOF
fi

if ! { cmd1 && cmd2 ; }; then echo ok ; fi
if ! {cmd1 && cmd2}; then echo ok ; fi
if ! cmd1 arg; then echo ok ; fi

case 1 in
2) echo xxx;
;;
?) foo || yyy ; foo abc || echo abc ;;
1) echo yyy;
esac

ls #should be outside of case 1 folding block

for i in `ls tests/auto/output/*.html`; do
    refFile=`echo $i | sed -e s,build,src, | sed -e s,output,reference, | sed -e s,.html,.ref.html,`
    cp -v $i $refFile
done

## >Settings >Configure Kate >Fonts & Colors >Highlitghing Text Styles >Scripts/Bash >Option >Change colors to some distinct color
## 1- In following line the -ucode should not be colored as option

pacman -Syu --needed intel-ucode grub
pacman -syu --needed intel-ucode grub

[[ $line_name =~ \{([0-9]{1,})\}\{([0-9]{1,})\}(.*) ]]
[[ $name =~ (.*)_(S[0-9]{2})(E[0-9]{2,3}[a-z]{0,1})_(.*) ]]
rm /data/{hello1,hello2}/input/{bye1,$bye2}/si{a,${b},c{k,p{e,a}}}/*.non
rm /data/{aa,{e,i}t{b,c} # Not closed
rm /data/{aa,{e,i}t{b,c}}
rm /data/{aa,{i}}
rm /data{aa{bb{cc{dd}}}}
rm /data{aaa`aaa}aa`aaa}a

# TODO `

# commands
abc
cp
:
.
:#nokeyword
path/cmd
ec\
ho
2
'a'c
$ab
${ab}c
\ a
!a
'a'[
\ [
!a[
a{}d
a{bc}d
a{b,c}d
a'b'c
a$bc
a${bc}d
a\ b
a!b

# commands + params
shortopt -ol -f/fd/fd -hfd/fds - -ol'a'b -f'a'/fd/fd -h'a'fd/fds
longopt --long-a --long-b=value --file=* --file=file* --file=dir/file
longopt --long-a'a'b --long'a'-b=value --fi'a'le=*
noopt 3 3d -f -- -f --xx dir/file
opt param#nocomment ab'a'cd ~a .a #comments
path path/file dir/ / // 3/f a@/ 'a'/b d/'a'b a\ d/f f/f\
ile
path ~ ~/ ~a/ . .. ./a ../a
path /path/* /path/f* /path/f@ /path/f@(|) {a/b} a{b}/c a/b{c} a/{b} a/{b}c
glob ? * ?f *f f* f? **/ ~/* ~* /path/f* 'a'* 'a'f/?
# ksh pattern is in conflict with extended pattern
extglob @ @(*) @(f*|f??(f)) f!(+(?(@(*(f)f)f)f)f)f @'a'@(|) a@(?)
echo *.*~(lex|parse).[ch](^D^l1)
echo /tmp/foo*(u0^@:t) *(W,X) *(%W)
subs f! f!! f!s 'a'!s \( $v {a,b} {a} {a}/d {a\,} {a,} {a,\},b} ds/{a,b}sa/s

ls !?main
ls $(echo NF)(:a)
ls ${(s.:.)PATH} | grep '^...s'
ls (#i)*.pmm
ls (#ia1)README
ls (*/)#bar
ls (../)#junk2/down.txt(:a)
ls (^(backup*|cache*|list*|tmp)/)##*(.)
ls (_|)fred.php
ls (dev*|fred*|joe*)/index*
ls (x*~x[3-5])
ls (xx|yy)
ls *(*@)
ls *(+nt)
ls *(.)^php~*.c~*.txt
ls *(.L-20)
ls *(.L0)
ls *(.Om[1,5])
ls *(.^m0)
ls *(.e#age 2017-10-01:00:00:00 2017-10-08:23:59:59#) /tmp
ls *(.e-age 2018/09/01 2018/01/01-)
ls *(.f644)
ls *(.g:root:)
ls *(.m-1)
ls *(.mM+6)
ls *(.mh+3)
ls *(.mh-3)
ls *(.mh3)
ls *(.mw+2)
ls *(.om[0,5]e-age 2017/09/01 2017/10/01-)
ls *(.om[2,$]) old/
ls *(.rwg:nobody:u:root:)
ls *(.u:apache:)
ls *(/)
ls *(/^F)
ls *(L0f.go-w.)
ls *(Lk+100)
ls *(Lm+2)
ls *(R)
ls *([1,10])
ls *(^/,f44?,f.gu+w.,oL+rand,oe:"$cmd -x":P:echo::h)
ls *(m4)
ls *(mh0)
ls *(mw3)
ls **.php
ls **/*(#ia2)readme
ls **/*(-@)
ls **/*(.)
ls **/*(.:g-w:)
ls **/*(.Lm+10)
ls **/*(D/e:'[[ -e $REPLY/index.php && -e $REPLY/index.html ]]':)
ls **/*(u0WLk+10m0)
ls **/*.(js|php|css)~(djr|libs|dompdf)/*~*/junk/*
ls **/*.(js|php|css)~(libs|locallibs|test|dompdf)/*
ls **/*.(php|inc)
ls **/*.(php|inc)~(libs|locallibs)/*(.OL[1,5])
ls **/*.txt(D.om[1,5])
ls **/*~*(${~${(j/|/)fignore}})(.^*)
ls **/*~*vssver.scc(.om[1,20])
ls **/*~pdf/*(.m0om[1,10])
ls **/^(vssver.scc|*.ini)(.om[1,20])
ls **/^vssver.scc(.om[1,20])
ls **/index.php~dev*(/*)##
ls **/main.{php,js,css}
ls *.(jpg|gif|png)(.)
ls *.*(e-age 2018/06/01 now-)
ls *.*(mM4)
ls *.*~(lex|parse).[ch](^D^l1)
ls *.*~[a-m]*(u:nobody:g:apache:.xX)
ls *.c(#q:s/#%(#b)s(*).c/'S${match[1]}.C'/)
ls *.c(:r)
ls *.c~lex.c
ls *.h~(fred|foo).h
ls *.{aux,dvi,log,toc}
ls *.{jpg,gif}(.N)
ls *[^2].php~*template*
ls *y(2|).cfm
ls *y2#.cfm
ls *~*.*(.)
ls ./*(Om[1,-11])
ls ./**/*(/od) 2> /dev/null
ls ./**/*.(php|inc|js)
ls ./**/*.{inc,php}
ls ./*.back(#qN)
ls ./{html,live}/**/*.(php|inc|js)~(**/wiki|**/dompdf)/*
ls /path/**/*(.a+10e{'stat -sA u +uidr $REPLY; f[$u]="$f[$u]$REPLY"'})
ls <-> <-6> <4-> <4-5> 0<-> {1..5} {2,3} {00..03} (4|5) [3-4]  [3-47-8] 0? ?2 *2
ls =some_file
ls DATA_[0-9](#c,4).csv
ls DATA_[0-9](#c3).csv
ls DATA_[0-9](#c4,).csv
ls DATA_[0-9](#c4,7).csv
ls PHP*/**/*.php
ls [01]<->201[45]/Daily\ report*.csv(e#age 2014/10/22 now#)
ls ^*.(css|php)(.)
ls ^?*.*
ls ^?*.*(D)
ls ^?*.[^.]*(D)
ls a(#c3).txt
ls file<20->
ls foot(fall)#.pl
ls fred<76-88>.pl
ls fred<76->.pl
ls fred^erick*
ls fred{09..13}.pl
ls fred{joe,sid}.pl
ls x*~(x3|x5)
ls x*~^x[3,5]
ls x*~x[3,5]
ls x^[3,5]
ls y2#.cfm y{2,}.cfm y(2|).cfm {y2,y}.cfm (y|y2).cfm y*.cfm
ls {^dev*,}/index.php(.N)
ls {_,}fred.php
ls {p..q}<5->{1..4}.(#I)php(.N)
ls ~1/*(.om[1])
ls **/*.php~*junk*/*  #find all calls to mail, ignoring junk directories
ls **/(*.cfm~(ctpigeonbot|env).cfm)
ls **/*.{js,php,css}~(libs|temp|tmp|test)/*
ls */*.php~libs/*~temp/*~test/*
ls **/(*.cfm~(ctpigeonbot|env).cfm)~*((#s)|/)junk*/*(.)
ls **/*.(js|php|css)~(libs|temp|test)/*
ls **/*.(js|php|css)~libs/*~temp/*~test/*
ls report/**/*.{inc,php}  # searching for a php variable
ls *.log(Ne-age 2006/10/04:10:15 2006/10/04:12:45-)
ls $(echo /c/aax/*(.om[1]))(+cyg) &
ls *~vssver.scc(.om[1])
ls /c/aax/*(.om[1]+cyg)
ls ${(ps:\0:)"$(grep -lZ foobar ./*.txt(.))"}
ls [[[[]]x*

2 - f -f
!a -f
'a' -f
$a -f

# redirections (prefix)
<<<s cat
<<<'s' cat
<<<'s's cat
<<<s's's cat
<<<s${s}s cat
<<< s${s}s cat
>&2 cat
<f cat
2>3 cat
2>&3 cat
2>& 3 cat
2>f cat
&>f cat
2>>(xless) cat
2<<(xless) cat
2>>(xless)cat
2<<(xless)cat

# redirections
cat f>2
cat d/f>2
cat d/f >2
cat d/f >& 2
cat >2 d/f
cat > 2
cat <(echo) <(echo a) <(echo a/f) <(echo ) <(echo a ) <(echo a/f )
cat 2>>(xless)
cat 2<<(xless)
cat 2>&1 &>f &>>f 2<&1- 2<>f 2<<heredoc
bla bla
heredoc
<<-'h' cat
bla
h
<<"'" cat
bla
'

# branches
cat a|cat
cat a&cat
cat a||cat
cat a&&cat
cat a;cat
cat a | cat
cat a & cat
cat a || cat
cat a && cat
cat a ; cat
cat a'a';cat

# substitutions
echo '' 'a' '\' "" "a" "\\" "$a" "a""a"'a''a' a'b'c a"b"c a$'\n'c
echo a!bc a{a}b a{b,c}d a{b,{d,e}}d a\ b
echo a$bc a$b/c a${b}c a$((b-3))c a$(b)c a$(a b c)c
echo ${a[*]} ${a[@]} ${a[${b}]} ${a:-x$z} ${a/g} ${a//f/f} ${a//f*/f*}
echo ${!a} ${#a[1]} ${a:1:$b} $((++i,i--))
echo ${a:^v} ${=a:/#%a#?*/bla} ${x#??(#i)} ${das:-{}<a.zsh}
echo ${(f)"$(<$1)"} ${${(Az)l}[$2]} ${(f)"$(eval ${(q)@[2,$]})"}
echo ${(@)foo} ${(@)foo[1,2]} ${${(A)name}[1]} ${(AA)=name=...} ${(Q)${(z)foo}}
echo ${(ps.$sep.)val} ${(ps.${sep}.)val} ${(s.$sep.)val} ${(s.)(.)val}
echo ${(pr:2+3::_::$d:)var} ${(r:2+3::_::$d:)var}
echo ${${:-=cat}:h}
$foo:h34:a:gs/dfs/fds/:s/fds/d'd'f xyz $foo: $foo:O $foo:A
3=$foo:QQQ xyz $a[3,$]:h3:t1:e
echo ${${~foo}//\*/*.c}
echo !$ !!:$ !* !!:* !-2:2 !:-3 !:2* !:2- !:2-3 !^ !:1 !!:1
echo "$bg[blue]$fg[yellow]highlight a message"
echo "$bg[red]$fg[black]${(l:42::-:)}"
echo "${${(@)foo[2,4]}[2]}"
echo "${(j::)${(@Oa)${(s::):-hello}}}"
echo "${(j::)${(@Oa)${(s::):-hello}}}"
echo "<a href='$url'>$anchortext</a>"
echo $(( sin(1/4.0)**2 + cos(1/4.0)**2 - 1 ))
echo $a[${RANDOM}%1000] $a[${RANDOM}%11+10]
echo $convtable[158]
echo ${array[0]: -7 : +  22  }  ${array[1]: num  }
echo ${parameter##word} ${parameter%%word}
echo $f ' # $fred'
echo $f:e $f:h $f:h:h $f:r $f:t $f:t:r $file:r
echo ${(C)foo:gs/-/ /:r} ${(M)0%%<->} ${(j/x/s/x/)foo} ${(l:$COLUMNS::-:)}
echo ${(l:3::0:)${RANDOM}} ${(s/x/)foo%%1*} ${0##*[!0-9]}
echo ${a:2:2} ${a:2} ${a[1,3]} ${d/#?/} ${d/%?/} ${d[1,-2]} ${d[2,$]}
echo ${d[2,-1]} ${file##*/} ${file%.*} ${texfilepath%/*.*} *(f:u+rx,o-x:)
echo *(f:u+rx:) **/*(@-^./=%p) **/*(@-^./=%p) convert_csv.php(:a)
cd $(locate -l1 -r "/zoo.txt$")(:h) # cd to directory of first occurence of a file zoo.txt
cd ${$(!!)[3]:h}  # cd to 3rd in list
cd ${$(locate zoo.txt)[1]:h}
cd ${drive}/inetpub/wwwdev/www.some.co.uk/
cd **/*.php(.om[1]:h) # cd to directory of newest php file
cd -
cd /tmp/test/;touch {1..5} {6,7,8,12} {00..03}
cd ~www/admin
chmod g+w **/*
chmod someuser /**/*(D^u:${(j.:u:.)${(f)"$(</etc/passwd)"}%%:*}:)
cp *.mp3(mh-4) /tmp # copy files less than 4 hours old
cp -a file1 file   # -a transfer  permissions etc of file1 to file2preserve
file **/*(D@) | fgrep broken
file **/*(D@) | fgrep broken
file=${1/#\//C:\/} # substitute / with c:/ Beginning of string
file=${1/%\//C:\/} # substitute / with c:/ End of string
file=${1/\//C:\/} # substitute / with c:/ ANYWHERE in string
filelst+=($x)
filelst[$(($#filelst+1))]=$x
files=(${(f)"$(egrepcmd1l)"} )
files=(${(f)"$(ls *$**)"}(.N)) # ")`
files=(**/*(ND.L0m+0m-2))
mkdir $f:h;touch $f
mv Licence\ to\ Print\ Money.pdf !#^:gs/\\ //
path=(${path:#$path_to_remove})
path=(${path:|excl})
pattern=${(b)str}
pattern=${(q)str}
print "$aa[one\"two\"three\"quotes]"
print "$bg[cyan]$fg[blue]Welcome to man zsh-lovers" >> $TTY
print $(( [#8] x = 32, y = 32 ))
print $((${${(z)${(f)"$(dirs -v)"}[-1]}[1]} + 1)) # or
print $(history -n -1|sed 's/.* //')
print $aa[(e)*]
print $ass_array[one]
print $x $y
print ${#path[1]}       # length of first element in path array
print ${#path}          # length of "path" array
print ${$( date )[2,4]} # Print words two to four of output of ’date’:
print ${$(/sbin/ifconfig tun0)[6]}
print ${${$( LC_ALL=C /sbin/ifconfig lo )[6]}#addr:}
print ${${$(LC_ALL=C /sbin/ifconfig eth0)[7]}:gs/addr://}
print ${${(Cs:-:):-fred-goat-dog.jpg}%.*}
print ${${(z)$(history -n -1)}[-1]}
print ${${(z)history[$((HISTCMD-1))]}[-1]}
print ${(L)s// /-}.jpg
print ${(L)s:gs/ /-/}.jpg
print ${(S)foo//${~sub}/$rep}
print ${(k)ass_array} # prints keys
print ${(v)ass_array} # prints values
print ${JUNK/%./_}                 # substitute last . for a _
print ${JUNK/.(#e)/_}              # substitute last . for a _
print ${arr//(#m)[aeiou]/${(U)MATCH}}
print ${array:t}
print ${foo%%$'\n'}                # strip out a trailing carriage return
print ${foo//$'\n'}                # strip out any carriage returns (some systems use \r)
print ${foo//${~sub}/$rep}
print ${foo: 1 + 2}
print ${foo:$(( 1 + 2))}
print ${foo:$(echo 1 + 2)}
print ${foo:3}
print ${param:&}   (last substitute)
print ${somevar//[^[:alnum:]]/_}   # replace all non-alphanumerics with _ the // indicates global substitution
print ${string[(r)d?,(r)h?]}
print '\e[1;34m fred'
print (*/)#zsh_us.ps
print *(e:age 2006/10/04 2006/10/09:)
print **/*(/^F) | xargs -n1 -t rmdir #delete empty directories
print *.c(e_'[[ ! -e $REPLY:r.o ]]'_)
print -C 1 $X           # print each array element on it's own line
print -l "${(s.:.)line}"
print -l $MATCH X $match
print -l $accum
print -l *(n:t)      # order by name strip directory
print -l **/*(-@)
print -l **/*(On:t)  # recursive reverse order by name, strip directory
print -r -- $^X.$^Y
print -r -- ${(qq)m} > $nameoffile      # save it
print -rC1 /tmp/foo*(u0^@:t)
print -rC1 b*.pro(#q:s/pro/shmo/)(#q.:s/builtin/shmiltin/)
print -rC2 -- ${1:[...]}/*(D:t)
print -rl $HOME/${(l:20::?:)~:-}*
print -rl -- ${${=mapfile[/etc/passwd]}:#*(#i)root*}
print -rl /**/*~^*/path(|/*)
print {$((##n))..$((##y))}P\ 10P | dc
print root@192.168.168.157:${PWD/test/live}v


# conditions
[ a ]
[ -f f'f'f ]
[ -f f]'f'f] ]
[ -t 13 ]
[ -t 13] ]
[ -t 13] ]
[ -v abc ]
[ -z abc ]
[ abc -ef abc ]
[ abc -ef abc ]
[ abc-ef -ef abc-ef ]
[ abc == abc ]
[ abc < abc ]
[ abc -eq abc ]
[[ abc -eq abc ]]
[ 1+2 -eq 1+2 ]
[[ 1+2 -eq 1+2 ]]
[ a = b c ]
( [ a = b ] )
([ a = b ])
[[ a = b c ]]
[[ x =~ a(b c|$)' '{1,}[a[.digit.]] ]]
[[ x =~ [ ] ]]
[[ x =~ ([ ]) ]]
[[ x =~ [ ]]
[[ x =~ ([) ]]
[[ a<b ]]
[[ a <b ]]
[[ a< b ]]
[[ a < b ]]
[[(! -d .)]]
[[ ! -d . ]]
[[ !(-d .) ]]
[[ -f a || -f b ]]
[[ -f a||-f b ]]
[ -d `echo .`] ]
[[ -d `echo .`]] ]]
[[ a != b && ${a}a = b${b} ]]
[[ $1 == <-> ]]
[[ a =~ <1-2>a<->a<-2>a<2->a([!d]a?s[x[:alnum:]]|d?)p ]]
[[ -n file*(#qN) ]]
[[ ( -f foo || -f bar ) && $report = y* ]] && print File exists.
[[ $str = ${~pattern} ]]
[[ $str = ${~pattern} ]]
[[ "?" = ${(~j.|.)array} ]]
( [[ a = b ]] )
([[ a = b ]])


[ a -eq 2 ] || [ a -eq 2] ] && [[ a -eq 2 ]] || [[ a != b ]];
[ a -eq 2 ]||[ a -eq 2] ]&&[[ a -eq 2 ]]||[[ a != b ]];

((3+1+a+$c*(x) & 0x4342_2fd+03-08_5/23#D9a@_^8))
((1.3/(2-(a-4))))

# they are not arithmetic evaluations...
((cmd && cmd) || cmd)
$((cmd && cmd) || cmd)
((cmd &&
cmd) || cmd)
$((cmd &&
cmd) || cmd)

print $(( [#_] sqrt(1e7) 0__39 1423e23 .2443 43.34 34.43e4 .d))

{ echo
    echo
}
{ echo ; }
{ echo }
{echo}
{ls f} always {ls}
{echo {a}}
}echo
echo {a} {a/b} a{b}/c a/b{c} a/{b} a/{b}c d/{{a}}
echo {a{a{a}}}
echo {a{a{a}a}a}a
echo {a
echo a}
echo{a}
echo{a{a{a}}}
echo{a{a{a}a}a}a
echo{a
echo}

{ {echo a} }
{ {echo a}a} }
{ { echo a } }
{ { echo a}a } }

{ {echo a/b} }
{ {echo a/b}a} }
{ { echo a/b } }
{ { echo a/b}a } }

{ {echo >a/b} }
{ {echo >a/b}a} }
{ { echo >a/b } }
{ { echo >a/b}a } }

{ab}c}
{a,b}c}
{ab}[}
{a,b}[}

cat >f{oo,ar}

(echo ; echo)
(echo
    echo)
(echo a)
test a -eq b

# functions
a() { echo x; }
a  () { echo x; }
function f { echo x; }
kde.org() { echo x; }
--func() { echo x; }

# variables
a=(a b c)
a='a'
a+=b
a[1]='a'
a[$i]='x'
a[$((
    2+4
))]='x'
a=([a]=2 `echo` -s > 'ds')
a=(#comment
value#nocomment #comment)
)
a=a cat
a=`ls` cat
a[2+3][d]=5

# control structure
for name in a b c {d,e} ; do echo ; done
for name; do echo ; done
for name do echo ; done
for ((i=0;i<5;++i)) ; do echo $i ; done
for ((i=1;$#A[i];i++)) echo $A[$i]
for c ({1..50}) {php ./test.php; sleep 5;}
for count in {1..10}; do echo $count ; done
for f (*(.)) mv $f fixed_$f
for f (**/x) cp newx $f
for f (*.txt) { echo $f }
for f in **/x; do;cp newx $f; done
for f in */include/dbcommon.php; do;cp dbcommon.php $f; done
for file (*(ND-.)) IFS= read -re < $file
for i (./*.mp3){mpg321 --w - $i > ${i:r}.wav}
for i in *(.); mv $i ${i:u} # `bar to `BAR
for i in **/*(D@); [[ -f $i || -d $i ]] || echo $i
for i in **/*.gif; convert $i $i:r.jpg
for i in {3,4}; sed s/flag=2/flag=$i/ fred.txt > fred$i.txt
for ip ({217..219} 225) {echo -n $ip ;ping -n 1 11.2.2.$ip| grep Received}
for user (${(k)f}) {print -rn $f[$user]|mailx -s "..." $user}
for x ( 1 2 {7..4} a b c {p..n} *.php) {echo $x}
select name in a ; do echo ; done
select name; do echo ; done
if : ; then echo ; elif [[ : ]] ; then echo ; else echo ; fi
if [ $# -gt 0 ];then string=$*;else;string=$(getclip);fi
if [ $# -gt 0 ];then string=$*;else;string=$(getclip);fi # get parameter OR paste buffer
if [[ (($x -lt 8) && ($y -ge 32)) || (($z -gt 32) && ($w -eq 16)) ]] ; then print "complex combinations"; fi
if builtin cd $1 &> /dev/null ; then echo ; fi
if grep -iq 'matching' *.php ;then echo "Found" ;else echo "Not Found"; fim=("${(@Q)${(z)"$(cat -- $nameoffile)"}}") fi
while : || : ; do echo ; done
while (true){echo -n .;sleep 1}
while (true){echo .;sleep 1}
while true ;do date; sleep 5; done # forever
while true; do echo "infinite loop"; sleep 5; done
until : ; : ; do echo ; done
case a in a) esac
case a in a) echo ; esac
case pwd in (patt1) echo ; echo ;; (patt*) echo ;& patt?|patt) echo ;|
patt) echo ;; esac
repeat 1+2+`echo 1`+23 do echo pl; done
repeat 3 time sleep 3   # single command
repeat 5 ;do date; sleep 5; done # multi
foreach x y z ( a `a b`; c ) echo ;end
for x y ( a b bc d ds ) echo $x $y
for x y in a b c ; echo $x $y
for x y ; echo $x $y
case w { a) echo ;& (b?) echo }
case a in
#a) echo ;;
a#) echo ;;
esac

for name in a
 b c ;
do
echo
done

case a in
  a\( | b*c? ) echo
  (b$c) # no pattern
  ;;
  (b$c) ;;
  # no pattern
  (b$c)
esac

case "$1" in
 "a") run_a|&a;;
 "b") run_b;;
 "c") run_c;;
 *) echo "Plase choose between 'a', 'b' or 'c'" && exit 1;;
esac

case $ans in
 1|a) sdba $key;;
 2|f) sdbf $key;;
 3|i) sdbi $key;;
 *) echo "wrong answer $ans\n" ;;
esac

case "$ans" in
 2|${prog}9) cd "$(cat /c/aam/${prog}9)" ;;
 **) echo "wrong number $ans\n" ;;
esac

select f in $(ls **/*.tex |egrep -i "${param}[^/]*.tex")
do
 if [[ "$REPLY" = q ]]
 then
    break
 elif [[ -n "$f" ]]; then
    gvim $f
 fi
done

for d (. ./**/*(N/m-2)) {
  print -r -- $'\n'${d}:
  cd $d && {
     l=(*(Nm-2))
     (($#l)) && ls -ltd -- $l
     cd ~-
  }
}

for f in http://zsh.sunsite.dk/Guide/zshguide{,{01..08}}.html; do
    lynx -source $f >${f:t}
done

for f in ./**/*(-@); do
    stat +link -A l $f
    (cd $f:h & [[ -e $l.gz ]]) & ln -sf $l.gz $f
done

for ((i=1; i <= $#fpath; ++i)); do
    dir=$fpath[i]
    zwc=${dir:t}.zwc
    if [[ $dir == (.|..) || $dir == (.|..)/* ]]; then
        continue
    fi
    files=($dir/*(N-.))
    if [[ -w $dir:h && -n $files ]]; then
        files=(${${(M)files%/*/*}#/})
        if ( cd $dir:h &&
            zrecompile -p -U -z $zwc $files ); then
        fpath[i]=$fpath[i].zwc
        fi
    fi
done

if ztcp pwspc 2811; then
    tcpfd=$REPLY
    handler() {
        zle -I
        local line
        if ! read -r line <&$1; then
            # select marks this fd if we reach EOF,
            # so handle this specially.
            print "[Read on fd $1 failed, removing.]" >&2
            zle -F $1
            return 1
        fi
        print -r - $line
    }
    zle -F $tcpfd handler
fi

while [[ $? -eq 0 ]] do
    b=($=ZPCRE_OP)
    accum+=$MATCH
    pcre_match -b -n $b[2] -- $string
done

# bug #380229
${str:$((${#a[1]}+1))}

# from http://zshwiki.org/home/examples/hardstatus
function title {
  if [[ $TERM == "screen" ]]; then
    # Use these two for GNU Screen:
    print -nR $'\033k'$1$'\033'\\

    print -nR $'\033]0;'$2$'\a'
  elif [[ $TERM == "xterm" || $TERM == "rxvt" ]]; then
    # Use this one instead for XTerms:
    print -nR $'\033]0;'$*$'\a'
  fi
}

function precmd {
  title zsh "$PWD"
}

function preexec {
  emulate -L zsh
  local -a cmd; cmd=(${(z)1})
  title $cmd[1]:t "$cmd[2,-1]"
}

function ddump(){diff -w ~dump/"$1" "$1"}   # diff local file with new one in dump
function g{0..9} { gmark $0 $* }          # declaring multiple functions
function hello_function { echo "hello world" ; zle .accept-line}
function scd(){setopt nonomatch;e=/dev/null;cd $1(/) &> $e||cd $1*(/) &> $e||cd *$1(/) &> $e||cd *${1}*(/) &> $e||echo sorry}
function vx{0..9} {gvim.exe c:/aax/${0/#v/} &}
function {xyt,xyy} { if [ "$0" = "xyy" ]; then echo run xyy code; else  echo run xyt code; fi ; echo run common code } #

# creating a family of functions
# generate hrefs from url
function href{,s}
{
    # href creates an HTML hyperlink from a URL
    # hrefs creates an HTML hyperlink from a URL with modified anchor text
    PROGNAME=`basename $0`
    url=`cat /dev/clipboard`
    if [ "$PROGNAME" = "href" ] ; then
        href="<a href='$url'>$url"
    elif [ "$PROGNAME" = "hrefs" ] ; then
        anchortext=${${(C)url//[_-]/ }:t}
        href="<a href='$url'>$anchortext"
    fi
    echo -n $col
    echo $href > /dev/clipboard | more
}

# create vim scratch files va,vb to vz
function vx{a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,q,r,s,t,u,v,w,x,y,z}
{
    scratchfile=${0/#v/}
    gvim.exe c:/aax/$scratchfile &
}

VDF(){cd *(/om[1]);F=$(echo *(.om[1]));vi $F}
cyg(){reply=("$(cygpath -m $REPLY)")}
f  (){for i; do echo $i;done}
fg_light_red=$'%{\e[1;31m%}'
fn() { setopt localtraps; trap '' INT; sleep 3; }
nt() { [[ $REPLY -nt $NTREF ]] }
preexec(){ echo using $@[1]}
take(){[ $# -eq 1 ]  && mkdir "$1" && cd "$1"} # create a directory and move to it in one go

caption always "%3n %t%? (%u)%?%?: %h%?"

preexec() {
  emulate -L zsh
  local -a cmd; cmd=(${(z)1})             # Re-parse the command line

  # Construct a command that will output the desired job number.
  case $cmd[1] in
      fg)
        if (( $#cmd == 1 )); then
          # No arguments, must find the current job
          cmd=(builtin jobs -l %+)
        else
          # Replace the command name, ignore extra args.
          cmd=(builtin jobs -l ${(Q)cmd[2]})
        fi;;
       %*) cmd=(builtin jobs -l ${(Q)cmd[1]});; # Same as "else" above
       exec) shift cmd;& # If the command is 'exec', drop that, because
          # we'd rather just see the command that is being
          # exec'd. Note the ;& to fall through.
       *)  title $cmd[1]:t "$cmd[2,-1]"    # Not resuming a job,
          return;;                        # so we're all done
      esac

  local -A jt; jt=(${(kv)jobtexts})       # Copy jobtexts for subshell

  # Run the command, read its output, and look up the jobtext.
  # Could parse $rest here, but $jobtexts (via $jt) is easier.
  $cmd >>(read num rest
          cmd=(${(z)${(e):-\$jt$num}})
          title $cmd[1]:t "$cmd[2,-1]") 2>/dev/null
}

function precmd() {
  title zsh "$IDENTITY:$(print -P %~)"
}

"%{^[]0;screen ^En (^Et) ^G%}"

print -nRP $'\033k%(!.#\[.)'$1$'%'$\(\(20\-${#1}\)\)$'< ..<'${${2:+${${${@[${#${@}}]##/*/}/#/ }:-}}//\"/}$'%(!.\].)\033'\\

c() { echo -E "$(<$1)" }
col() { for l in ${(f)"$(<$1)"} ; echo ${${(Az)l}[$2]} }
colx() { for l in ${(f)"$(eval ${(q)@[2,$]})"} ; echo ${${(Az)l}[$1]} }

[[ -r /etc/ssh/ssh_known_hosts ]] && _global_ssh_hosts=(${${${${(f)"$(</etc/ssh/ssh_known_hosts)"}:#[\|]*}%%\ *}%%,*}) || _global_ssh_hosts=()
_ssh_hosts=(${${${${(f)"$(<$HOME/.ssh/known_hosts)"}:#[\|]*}%%\ *}%%,*}) || _ssh_hosts=()
_ssh_config=($(cat ~/.ssh/config | sed -ne 's/Host[=\t ]//p')) || _ssh_config=()
: ${(A)_etc_hosts:=${(s: :)${(ps:\t:)${${(f)~~"$(</etc/hosts)"}%%\#*}##[:blank:]#[^[:blank:]]#}}} || _etc_hosts=()

prefix='(I:'$@[$(($i+1))]':)'$prefix || prefix='${('$tmp'I:'$@[$(($i+1))]':'${prefix[$(($#tmp+4)),-1]}
prefix='${'${j:+($j)}$prefix; suffix+=':#'${@[$(($i+1))]//(#m)[\/\'\"]/\\$MATCH}'}'
cmd+='<'${(q)@[$(($i+1))]}';'
C=${OPTARG//(#m)[[\/\'\"\\]/\\$MATCH}
$=p $e'"$(<'${(j:<:)${(q)@}}')"'$m

zshaddhistory() {
    print -sr -- ${1%%$'\n'}
    fc -p .zsh_local_history
}

TRAPINT() {
    print "Caught SIGINT, aborting."
    return $(( 128 + $1 ))
}

zsh_directory_name() {
    emulate -L zsh
    setopt extendedglob
    local -a match mbegin mend
    if [[ $1 = d ]]; then
        # turn the directory into a name
        if [[ $2 = (#b)(/home/pws/perforce/)([^/]##)* ]]; then
            typeset -ga reply
            reply=(p:$match[2] $(( ${#match[1]} + ${#match[2]} )) )
        else
            return 1
        fi
    elif [[ $1 = n ]]; then
        # turn the name into a directory
        [[ $2 != (#b)p:(?*) ]] && return 1
        typeset -ga reply
        reply=(/home/pws/perforce/$match[1])
    elif [[ $1 = c ]]; then
        # complete names
        local expl
        local -a dirs
        dirs=(/home/pws/perforce/*(/:t))
        dirs=(p:${^dirs})
        _wanted dynamic-dirs expl 'dynamic directory' compadd -S\] -a dirs
        return
    else
        return 1
    fi
    return 0
}

() {
    print File $1:
    cat $1
} =(print This be the verse)

if [[ $foo = (a|an)_(#b)(*) ]]; then
    print ${foo[$mbegin[1],$mend[1]]}
fi

zshaddhistory() {
    emulate -L zsh
    ## uncomment if HISTORY_IGNORE
    ## should use EXTENDED_GLOB syntax
    # setopt extendedglob
    [[ $1 != ${~HISTORY_IGNORE} ]]
}

pick-recent-dirs-file() {
    if [[ $PWD = ~/text/writing(|/*) ]]; then
        reply=(~/.chpwd-recent-dirs-writing)
    else
        reply=(+)
    fi
}

run-help-ssh() {
    emulate -LR zsh
    local -a args
    # Delete the "-l username" option
    zparseopts -D -E -a args l:
    # Delete other options, leaving: host command
    args=(${@:#-*})
    if [[ ${#args} -lt 2 ]]; then
        man ssh
    else
        run-help $args[2]
    fi
}

local -A zdn_top=(
    g   ~/git
    ga  ~/alternate/git
    gs  /scratch/$USER/git/:second2
    :default: /:second1
)

(( $#files > 0 )) && print -rl -- $files | \
    mailx -s "empty files" foo [at] bar.tdl

print -r -- $s[3] ${(l:4:)s[4]} ${(l:8:)s[5]} \
    ${(l:8:)s[6]} ${(l:8:)s[8]} $s[10] $f ${s[14]:+-> $s[14]}

paste <(cut -f1 file1) <(cut -f3 file2) |
    tee >(process1) >(process2) >/dev/null

ls \
> x*

sed '
 s/mvoe/move/g
 s/thier/their/g' myfile


trap '
    # code
    ' NAL

!! # previous command
!!:0 !^ !:2 !$ !#$ !#:2 !#1 !#0
!!:gs/fred/joe/       # edit previous command replace all fred by joe
!!:gs/fred/joe/       # edit previous command replace all fred by joe
!!:s/fred/joe/        # Note : sadly no regexp available with :s///
!!:s/fred/joe/        # edit previous command replace first fred by joe
!$ (last argument of previous command)
!$:h (last argument, strip one level)
!$:h:h (last argument, strip two levels)
!-2 # command before last
!1 # oldest command in your history
!42                   # Re-execute history command 42
!42:p
!?echo
!?saket?:s/somefile1/somefile2/

(($#l)) && ls -ltd -- $l
((val2 = val1 * 2))
(mycmd =(myoutput)) &!
: *(.e{'grep -q pattern $REPLY || print -r -- $REPLY'})
: > /apache/access.log  # truncate a log file
< readme.txt
A=(1 2 5 6 7 9) # pre-populate an array
C:\cygwin\bin\mintty.exe -i /Cygwin-Terminal.ico /bin/zsh --login
C=3 && F=$(print *(.om[1,$C])) && for f ($(print $F)){php -l $f} && scp -rp $(print $F) user@192.168.1.1:$PWD
EDITOR='/bin/vim'
FILE=$(echo *(.om[1])) && ls -l $FILE && ssh 192.168.1.1 -l root "zsh -c 'ls -l $PWD/$FILE'"
FILES=( .../files/* )
IFS=$'\n\n'; print -rl -- ${(Oau)${(Oa)$(cat file;echo .)[1,-2]}}
IPREFIX=${PREFIX%%\=*}=
PREFIX=${PREFIX#*=}
PROMPT3="Choose File : "
PROMPT="%{$bg[cyan]%}%% "
PS3="$fg_light_red Select file : "
REPORTTIME=10 # Automatically /Report CPU usage for commands running longer than 10 seconds
RPROMPT="[%t]" (display the time)
X=(x1 x2)
Y=(+ -)
[[ "$(< $i)" = *\((${(j:|:)~@})\)* ]] && echo $i:h:t
[[ $OSTYPE == (#i)LINUX*(#I) ]];
[[ 'cell=456' =~ '(cell)=(\d+)' ]] && echo $match[1,2] $MATCH
[[ -e $L/config.php ]] && cp -p -update $T/config.php $L
[[ -n ${key[Left]} ]] && bindkey "${key[Left]}" backward-char
[[ 1 = 0 ]] && echo eq || echo neq
[[ alphabetical -regex-match ^a([^a]+)a([^a]+)a ]] &&
^chim^&-&ney-&-&-cheree # reuse LHS
^fred^joe             # edit previous command replace fred by joe
^php^cfm          # modify previous command (good for correcting spellos)
^str1^str2^:G         # replace as many as possible
^str1^str2^:u:p       # replace str1 by str2 change case and just display
a=(**/*(.D));echo $#a  # count files in a (huge) hierarchy
a=(1 2 3 4); b=(a b); print ${a:^b}
a=(a b); b=(1 2); print -l "${a:^b}"; print -l "${${a:^b}}"
a=12345
aa[(e)*]=star
accum=()
alias '..'='cd ..'
alias -g ...='../..'
alias -g NF='*(.om[1])' # newest file
alias gcd="cd $MCD"  # double quote stops once only evaluation
alias mcd="MCD=$(pwd)"  # double quote stops once only evaluation
anchortext=${${(C)url//[_-]/ }:t}  # titlecase
arr=(veldt jynx grimps waqf zho buck)
array=(~/.zshenv ~/.zshrc ~/.zlogout)
autoload edit-command-line
autoload -Uz up-line-or-beginning-search
autoload colors ; colors
bindkey "^N" most-recent-file
bindkey -s "^[OS" "\^d\^c\n"
bindkey -s "^[[18~" "ls -l\n"
c=(*.c) o=(*.o(N)) eval 'ls ${${c:#(${~${(j:|:)${o:r}}}).c}:?done}'
cd !$:h
cd !?ls
diff <(find / | sort) <(cat /var/lib/dpkg/info/*.list | sort)
dpath=${upath/#\/c\//c:/}          # convert /c/path/ to c:\path\
drive=$([[ "$LOGNAME" != davidr ]] && echo '/o' || echo '/c') # trad way
drive=${${${LOGNAME:#davidr}:+/o}:-/c}                        # zsh way
egrep -i "^ *mail\(" **/*.php
eval "$1=$PWD"
eval "m=($(cat -- $nameoffile)"
feh $FILES[$RANDOM%$#FILES+1]
foo="twinkle twinkle little star" sub="t*e" rep="spy"
foo=$'bar\n\nbaz\n'
foo=fred-goat-dog.jpg
fred=$((6**2 + 6))      # can do maths
(( $# == 0 ));
[ "$p1" = "end" ] || [ "$p1" = "-e" ]
[ $# -gt 0 ]  # parameter cnt > 0 (arguments)
[ $cnt -eq 1 ]
[[ "$1" == [0-9] ]]  # if $1 is a digit
[[ "$p2" == *[a-zA-Z][a-zA-Z][a-zA-Z]* ]]  # contains at least 3 letters
[[ "$pwd" == *$site2* ]]
[[ "$url" = www* ]] # begins with www
[[ -e /c/aam/z$1 ]]  # file exists
p1 p2 p3
pcre_compile -m "\d{5}"
pcre_match -b -- $string
perl -ne 's/(<\/\w+>)/$1\n/g; print' < NF > $(print NF).txt
ps -p $$ | grep $$ | awk '{print $NF}'
r oldstr=newstr
r\m $(locate nohup.out)
read -r line <&$fd; print -r - $line
read ans ; # read in a parameter
setopt EXTENDED_GLOB   # lots of clever stuff requires this
source ${ZDOTDIR:-$HOME}/.zkbd/$TERM-$VENDOR-$OSTYPE
ssh -t root@192.18.001.001 'sh -c "cd /tmp && exec zsh -l"'
ssh 192.168.1.218 -l root "zsh -c 'for i (/usr/*(/)) {ls \$i }'"
sshpass -p myppassword scp -rp * user@18.128.158.158:${PWD/staging/release}
str=aa,bb,cc;print ${(j:,:)${(qq)${(s:,:)str}}} # quotify a string
tel blenkinsop | grep -o "[[:alnum:][:graph:]]*@[[:alnum:][:graph:]]*" # filter just an email address from a text stream (not zsh)
touch {t,p}{01..99}.{php,html,c}  # generate 600 test files
touch {y,y2}.cfm
trap - INT
typeset "aa[one\"two\"three\"quotes]"=QQQ
typeset -A aa
typeset -A ass_array; ass_array=(one 1 two 2 three 3 four 4)
typeset -A convtable
typeset -i 16 y
unsetopt XTRACE VERBOSE
unsetopt localtraps
upath=${wpath//\\/\/}              # convert backslashes to forward slashes (Dos to Unix
url='www.some.com/some_strIng-HERe'
val=a:b:c
var=133;if [[ "$var" = <-> ]] ; then echo "$var is numeric" ;fi
var=ddddd; [[ "$var" =~ ^d+$ ]] && echo matched || echo did not match
var=dddee; regexp="^e+$"; [[ "$var" =~ $regexp ]] && echo $regexp matched $var || echo $regexp did not match $var
vared -p "choose 1-3 : " -c ans
vared PATH
whence -vsa ${(k)commands[(I)zsh*]}  # search for zsh*
widget
wpath=${wpath//\//\\\\}            # substitute Unix / with dos \ slashes
x=$?
zmodload -F zsh/stat b:zstat
zsh -lxic : 2> >(grep "> alias 'web'")
{ paste <(cut -f1 file1) <(cut -f3 file2) } > >(process)
