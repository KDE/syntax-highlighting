Comments
NB. This is a single-line comment
NB. check regions overlapping priority: Abc 'str' _1 2.3 y u +/ &. do. =: NB. =.
NB. check parts of speech aren''t detected: ( ) = ~ ^: assert. =: {{ }}
NB. check special labels are detected: BUG DEPRECATED FIXME NOTE TEST TODO WARN
NB. * actorName annotationText
NB.(
NB. This is a foldable multi-line comment
NB. check regions overlapping priority: Abc 'str' _1 2.3 y u +/ &. do. =: NB. =.
NB. check parts of speech aren''t detected: ( ) = ~ ^: assert. =: {{ }}
NB. check special labels are detected: BUG DEPRECATED FIXME NOTE TEST TODO WARN
NB.)

Strings
''
''''
'This is quoted text'
'And this '' and this NB. and this' 'Yet another quoted text'
'check regions overlapping priority: ''str'' _1 2.3 y u +/ &. do. =: NB. =.'
'check parts of speech aren''t detected: ( ) = ~ ^: assert. =: {{ }}'
'check special labels aren''t detected: BUG DEPRECATED FIXME NOTE TEST TODO WARN'

Numbers
123 _123 36b_1a.z2 _36b_1a.z2 123x _123x 1r23 _1r_23 1.23 _1.23 1.2e3 _1.2e_3
 _. _ __ 1.2e3j4.5e_6 _1.2e3j_4.5e_6 _j_ __j_ _.j_. 1j2p3j4 _1j2p_3j4 _j3p4j_.

Nouns
a. a: m n x y
(0 : 0)
  This is a foldable multi-line noun
  check regions overlapping priority: 'str' _1 2.3 y u +/ &. do. =: NB. =.
  check parts of speech aren''t detected: ( ) = ~ ^: assert. =: {{ }}
  check special labels aren''t detected: BUG DEPRECATED FIXME NOTE TEST TODO WARN
)
Note ''
  This is a foldable multi-line noun
  check regions overlapping priority: 'str' _1 2.3 y u +/ &. do. =: NB. =.
  check parts of speech aren''t detected: ( ) = ~ ^: assert. =: {{ }}
  check special labels aren''t detected: BUG DEPRECATED FIXME NOTE TEST TODO WARN
)

Parenthesis
( )

Verbs
= < <. <: > >. >: _: __: + +. +: * *. *: - -. -: % %. %: ^ ^. $ $. $: ~. ~:
| |. |: , ,. ,: ; ;: # #. #: ! /: \: [ [: ] { {. {: {:: }. }: ". ": ? ?.
A. c. C. e. E. i. i: I. j. L. o. p. p.. p: q: r. s: u u. u: v v. x: Z:
_9: _8: _7: _6: _5: _4: _3: _2: _1: 0: 1: 2: 3: 4: 5: 6: 7: 8: 9:

Adverbs
~ / \ /. /.. \. ]: } b. f. M.

Conjuctions
^: . : :. :: ;. !. !: [. ]. " ` `: @ @. @: & &. &: &.: H. L: m. S: t.

Controls
assert. break. case. catch. catchd. catcht. continue. do. else. elseif. end. fcase.
for. for_abc. goto_abc. if. label_abc. return. select. throw. try. while. whilst.

Copula
a =: 123
(a) =: '123'
(a;b;c) =: 123
'a' =: '123'
'a b c' =: 123
'a b c' =. '123'
'`a b c' =. 123
('a';'b';'c') =: '123'

Combined
123 a+/b`1:@.c 'qwerty' x*:y
for_abc. i. 5 do. {{{{{ *:y }} 55 return. end.
a=. b =: 123 +/ # i. 10
a_b =: '123' [ c__d =. 4 : 0  NB. foldable definition begins
  x +/ y
)
e__12 =: verb define  NB. foldable definition begins
  if. x do.  NB. inner foldable region begins
    y=. y + x  NB. comment after code
    x +/ y
  else.  NB. inner foldable region begins
    y +/ y
  end.
)
c_d_=. {{)c i=. s=. 0  NB. foldable definition begins
  x u m u. v n v. y
  while. 10 > i do.    NB. inner foldable region begins
    s=. s + i
    i=. >: i
  end.
  s=. s , _1 }}&.> 11 22 33
