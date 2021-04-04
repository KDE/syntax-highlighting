#! /bin/sh
# This is a test script for the Katepart Bash Syntax Highlighting by
#	Wilbert Berendsen.  This is not runnable !!!


# The highlighting recognizes basic types of input, and has a few special cases that
# are all in FindCommands.  The main objective is to create really proper nesting of
# (multiline) strings, variables, expressions, etc.



# ============== Tests: ===============

# basic types:
echo 'single quoted string'
echo "double quoted string"
echo $'string with esc\apes\x0din it'
echo $"string meant to be translated"
echo "$"


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
echo f.{01..100..3} f.{Z..a..-3}
echo f.{01..100} f.{a..Z}
# no brace expansion
echo f.{..100} f.{1..Z} f.{a..Z..}


# special characters are escaped:
echo \(output\) \&\| \> \< \" \' \*


# variable substitution:
echo $filename.ext
echo $filename_ext
echo ${filename}_ext
echo text${array[$subscript]}.text
echo text${array["string"]}.text
echo ${!prefix*}
echo ${!redir}
echo short are $_, $$, $?, ${@}, etc.
echo ${variable/a/d}
echo ${1:-default}
echo ${10} $10a # 9 is max


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

# Braces (bug ##387915)
[[ $line_name =~ \{([0-9]{1,})\}\{([0-9]{1,})\}(.*) ]]
[[ $name =~ (.*)_(S[0-9]{2})(E[0-9]{2,3}[a-z]{0,1})_(.*) ]]
rm /data/{hello1,hello2}/input/{bye1,$bye2}/si{a,${b},c{k,p{e,a}}}/*.non
rm /data/{aa,{e,i}t{b,c} # Not closed
rm /data/{aa,{e,i}t{b,c}}
rm /data/{aa,{i}}
rm /data{aa{bb{cc{dd}}}}
rm /data{aaa`aaa}aa`aaa}a

${array[0]: -7 : +  22  }  ${array[1]: num  }
${parameter##word} ${parameter%%word} ${parameter^^pattern} ${parameter,,pattern} ${parameter@operator}

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
{ab}c
{a,b}c
'a'c
$ab
${ab}c
\ a
!a
{ab}[
{a,b}[
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
a{}[
a{bc}[
a{b,c}[
a'b'[
a\ [
a!b[

# commands + params
shortopt -ol -f/fd/fd -hfd/fds - -ol'a'b -f'a'/fd/fd -h'a'fd/fds
longopt --long-a --long-b=value --file=* --file=file* --file=dir/file
longopt --long-a'a'b --long'a'-b=value --fi'a'le=*
noopt 3 3d -f -- -f --xx dir/file
opt param#nocomment ab'a'cd [[ param ]] } { ~a .a #comments
path path/file dir/ / // 3/f a@/ 'a'/b d/'a'b a\ d/f f/f\
ile
path ~ ~/ ~a/ . .. ./a ../a
path /path/* /path/f* /path/f@ /path/f@(|) {a/b} a{b}/c a/b{c} a/{b} a/{b}c
glob ? * ?f *f f* f? **/ ~/* ~* /path/f* 'a'* 'a'f/?
extglob @ @(*) @(f*|f??(f)) f!(+(?(@(*(f)f)f)f)f)f @'a'@(|) a@(?)
subs f! f!! f!s 'a'!s \( $v {a,b} {a} {a}/d {a\,} {a,} {a,\},b} ds/{a,b}sa/s
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

# redirections
cat f>2
cat d/f>2
cat d/f >2
cat d/f >& 2
cat >2 d/f
cat > 2
cat <(echo) <(echo a) <(echo a/f) <(echo ) <(echo a ) <(echo a/f )
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
echo ${a[*]} ${a[@]} ${a[${b}]} ${a:-x$z} ${a-x} ${a/g} ${a//f/f} ${a//f*/f*}
echo ${a^^l*} ${a,} ${!a} ${#a[1]} ${a:1:$b} $((++i,i--))

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

((3+1+a+$c*(x) & 0x43422fd+03-085/23#D9a@_^8))
((1/(2-(a-4))))

# they are not arithmetic evaluations...
((cmd && cmd) || cmd)
$((cmd && cmd) || cmd)
((cmd &&
cmd) || cmd)
$((cmd &&
cmd) || cmd)

{ echo
    echo
}
{ echo ; }
(echo ; echo)
(echo
    echo)
(echo a)
[ a -eq 2 ] || [ a -eq 2] ] && [[ a -eq 2 ]] || [[ a != b ]];
[ a -eq 2 ]||[ a -eq 2] ]&&[[ a -eq 2 ]]||[[ a != b ]];
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

# errors
a a(s) a

# control structure
for name in a b c {d,e} ; do echo ; done
for name; do echo ; done
for name do echo ; done
for ((i=0;i<5;++i)) ; do echo $i ; done
select name in a ; do echo ; done
select name; do echo ; done
if : ; then echo ; elif [[ : ]] ; then echo ; else echo ; fi
while : || : ; do echo ; done
until : ; : ; do echo ; done
case a in a) esac
case a in a) echo ; esac
case pwd in (patt1) echo ; echo ;; (patt*) echo ;;& patt?|patt) echo ;&
patt) echo ;; esac

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
