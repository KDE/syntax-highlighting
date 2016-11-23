# comment with ALERT

''' multiline
    comment
    ###
    '''

def func(x):
    """ API docs """
    if x == 42:
        func()
        c1 = {}
        c2 = {
            2.4,
            0x42
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
