# comment with ALERT

''' multiline
    comment
    ###
    '''

a = 100_000
b = 856_264.65
c = 0x420_F52

def func(x):
    """ EXTENDED API docs """
    if x == 42:
        func()
        c1 = {}
        c2 = {
            2.4,
            0x42,
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


def test_f_literals():
    f'xy'
    f'x{bar}y'
    f'x{bar["baz"]}y'
    f'x{bar["baz"]}y\n'

    f'x{bar["baz"]!r}y\n'
    f'x{bar["baz"]:.2}y\n'

a = "Escapes in String \U12345678 \xAB \""
"Escapes in Doc/Comment String \u1234 \xAb \"\\"
''' Doc/Comment String \N{123} \''''
""" Doc/Comment String \17 \x1f \n \" \\"""
