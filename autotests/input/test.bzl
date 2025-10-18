# comment with ALERT

load("//:rules.bzl", "my_rule")

''' multiline
    comment
    ###
    '''

// alert check, no word delimiter check at start of word, bug 397719
#TODO

a = 100_000
b = 856_264.65
c = 0x420_F52

def func(x):
    """ EXTENDED API docs """
    if x == 42:
        my_rule()
        c1 = {}
        c2 = {
            2.4: "a",
            0x42: True,
            }
        a1 = []
        a2 = [
            "a", 032, (
                )]
    else:
        print("""multi
            line
            string""")
        print("single \
                continued line ")
        print('single line')
    return float(len(a2))

my_string = "Hello {name}, you are {age} years old".format(name, age)
my_bytes = b"Hello {regular_text_color}"

a = "Escapes in String \U12345678 \xAB \""
b = '''\'''
'''
c = '''\''''
d = b'\u1234a\xffé\12k\n\g\
s\
\"\ '
e = rb'\u1234aé\n\g\
'
e2 = r'\u1234aé\n\g\
'
f = b""" é """
g = b"""\""""
h = 'a' + \
    '\x12'
i = b"t\e's\t"
j = rb"t\e's\t"
k = r"\\" + \
rb'\\' + \
r"""\\""" + \
r'''\\'''

"Escapes in Doc/Comment String \u1234 \xAb \"\\"
''' Doc/Comment String \N{123} \''''
""" Doc/Comment String \17 \x1f \n \" \\"""

# Decimal, Hex, Binary, Octal
a = 1_2_34L
b = 0_0_00_0
c = 0xA_3f_43D
d = 0b0_0_1_01
e = 0o71_2_34
# Float
d = 1.1E+3
e = 1.E+3
f = .1E+3
g = 1E+3
h = 1.1
i = 1.
j = .1
k =  1
l = 1_0.e+3_22 + .2_1e2 + 11_1.
# Complex
m = 1.1E+3j
n = 1.E+3j
o = .1E+3j
p = 1E+3j
q = 1.1j
r = 1.j
s = .1j
t =  1j
u = 1_0.e+3_22j + .2_1e2j + 11_1.j

# Invalid numbers
aaa.123
aaa123
.0x123

# reserved keywords
as
assert
async
await
class
del
except
finally
from
global
import
is
nonlocal
raise
try
while
with
yield

"\\\\\\\\\\ENSURE THAT THIS AND THE FOLLOWING LINES ARE AT THE END OF THE FILE\\\\\\\\\\ \
"
a = 'otherwise the next line will be
stuck on the Error attribute'
br"Strings delimited by single quotes (' or ")
can NOT be defined across multiple lines
unless escaped with \
"
