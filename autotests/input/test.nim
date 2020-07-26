# Nim Sample file
# Obtained form: https://nim-by-example.github.io/

# Comment ALERT NOTE FIXME
#[ Multi-line
comment ]#

## Documentation comment
##[ Multi-line
    documentation comment ]##

import strformat

type
    Person = object
        name: string
        age: Natural # Ensures the age is positive

let people = [
    Person(name: "John", age: 45),
    Person(name: "Kate", age: 30)
]

for person in people:
    # Type-safe string interpolation,
    # evaluated at compile time.
    echo(fmt"{person.name} is {person.age} years old")

# Thanks to Nim's 'iterator' and 'yield' constructs,
# iterators are as easy to write as ordinary
# functions. They are compiled to inline loops.
iterator oddNumbers[Idx, T](a: array[Idx, T]): T =
    for x in a:
        if x mod 2 == 1:
            yield x

for odd in oddNumbers([3, 6, 9, 12, 15, 18]):
    echo odd

# Use Nim's macro system to transform a dense
# data-centric description of x86 instructions
# into lookup tables that are used by
# assemblers and JITs.
import macros, strutils

macro toLookupTable(data: static[string]): untyped =
    result = newTree(nnkBracket)
    for w in data.split(';'):
        result.add newLit(w)

const
    data = "mov;btc;cli;xor"
    opcodes = toLookupTable(data)

for o in opcodes:
    echo o

# Variables
proc getAlphabet(): string =
    var accm = ""
    for letter in 'a'..'z':  # see iterators
        accm.add(letter)
    return accm

# Computed at compilation time
const alphabet = getAlphabet()

# Mutable variables
var
    a = "foo"
    b = 0
    # Works fine, initialized to 0
    c: int

# Immutable variables
let
    d = "foo"
    e = 5
    # Compile-time error, must be initialized at creation
    f: float

# Works fine, `a` is mutable
a.add("bar")
b += 1
c = 3

# Compile-time error, const cannot be modified at run-time
alphabet = "abc"

# Compile-time error, `d` and `e` are immutable
d.add("bar")
e += 1

# Const
STRING_LITERAL(TMP129, "abcdefghijklmnopqrstuvwxyz", 26);

# Loops
import strutils, random

randomize()
let answer = random(10) + 1
while true:
    echo "I have a number from 1 to 10, what is it? "
    let guess = parseInt(stdin.readLine)

    if guess < answer:
        echo "Too low, try again"
    elif guess > answer:
        echo "Too high, try again"
    else:
        echo "Correct!"
        break

block busyloops:
    while true:
        while true:
        break busyloops

# Case Statements
case "charlie":
    of "alfa":
        echo "A"
    of "bravo":
        echo "B"
    of "charlie":
        echo "C"
    else:
        echo "Unrecognized letter"

case 'h':
    of 'a', 'e', 'i', 'o', 'u':
        echo "Vowel"
    of '\127'..'\255':
        echo "Unknown"
    else:
        echo "Consonant"

proc positiveOrNegative(num: int): string =
    result = case num:
        of low(int).. -1:
            "negative"
        of 0:
            "zero"
        of 1..high(int):
            "positive"
        else:
            "impossible"

echo positiveOrNegative(-1)

# items and pairs
type
    CustomRange = object
        low: int
        high: int

iterator items(range: CustomRange): int =
    var i = range.low
    while i <= range.high:
        yield i
        inc i

iterator pairs(range: CustomRange): tuple[a: int, b: char] =
    for i in range:  # uses CustomRange.items
        yield (i, char(i + ord('a')))

for i, c in CustomRange(low: 1, high: 3):
    echo c

# Operators
iterator `...`*[T](a: T, b: T): T =
    var res: T = T(a)
    while res <= b:
        yield res
        inc res

for i in 0...5:
    echo i

# Inline Iterators
iterator countTo(n: int): int =
    var i = 0
    while i <= n:
        yield i
        inc i

for i in countTo(5):
    echo i

# Closure Iterators
proc countTo(n: int): iterator(): int =
    return iterator(): int =
        var i = 0
        while i <= n:
            yield i
            inc i

let countTo20 = countTo(20)

echo countTo20()

var output = ""
# Raw iterator usage:
while true:
    # 1. grab an element
    let next = countTo20()
    # 2. Is the element bogus? It's the end of the loop, discard it
    if finished(countTo20):
        break
    # 3. Loop body goes here:
    output.add($next & " ")

echo output

output = ""
let countTo9 = countTo(9)
for i in countTo9():
    output.add($i)
echo output

# Procs
proc fibonacci(n: int): int =
    if n < 2:
        result = n
    else:
        result = fibonacci(n - 1) + (n - 2).fibonacci

# Operators
proc `$`(a: array[2, array[2, int]]): string =
    result = ""
    for v in a:
        for vx in v:
            result.add($vx & ", ")
        result.add("\n")

echo([[1, 2], [3, 4]])  # See varargs for
                        # how echo works

proc `^&*^@%`(a, b: string): string =
    ## A confusingly named useless operator
    result = a[0] & b[high(b)]

assert("foo" ^&*^@% "bar" == "fr")

# Generic Functions
# Not really good idea for obvious reasons
let zero = ""
proc `+`(a, b: string): string =
    a & b

proc `*`[T](a: T, b: int): T =
    result = zero
    for i in 0..b-1:
        result = result + a  # calls `+` from line 3

assert("a" * 10 == "aaaaaaaaaa")

# Blocks
block outer:
    for i in 0..2000:
        for j in 0..2000:
            if i+j == 3145:
                echo i, ", ", j
                break outer

let b = 3
block:
    let b = "3"  # shadowing is probably a dumb idea

# Primitive types
let
    a: int8 = 0x7F # Works
    b: uint8 = 0b1111_1111 # Works
    d = 0xFF # type is int
    c: uint8 = 256 # Compile time error
let
    a: int = 2
    b: int = 4
echo 4/2

# Types Aliases
type
    MyInteger* = int

let a: int = 2
discard a + MyInteger(4)

# Objects
type
    Animal* = object
        name*, species*: string
        age: int

proc sleep*(a: var Animal) =
    a.age += 1

proc dead*(a: Animal): bool =
    result = a.age > 20

var carl: Animal
carl = Animal(name : "Carl",
              species : "L. glama",
              age : 12)

let joe = Animal(name : "Joe",
                 species : "H. sapiens",
                 age : 23)

assert(not carl.dead)
for i in 0..10:
    carl.sleep()
assert carl.dead

# Enums
type
    CompassDirections = enum
        cdNorth, cdEast, cdSouth, cdWest

    Colors {.pure.} = enum
        Red = "FF0000", Green = (1, "00FF00"), Blue = "0000FF"

    Signals = enum
        sigQuit = 3, sigAbort = 6, sigKill = 9

# Distinct Types
type
    Dollars* = distinct float

var a = 20.Dollars
a = 25  # Doesn't compile
a = 25.Dollars  # Works fine

# Strings
echo "words words words ⚑"
echo """
<html>
  <head>
  </head>\n\n

  <body>
  </body>
</html> """

proc re(s: string): string = s

echo r"."".\s\"      # Raw string
echo re"\b[a-z]++\b" # Regular expression
echo function"text"  # Tagged string

# Arrays
type
    ThreeStringAddress = array[3, string]
let names: ThreeStringAddress = ["Jasmine", "Ktisztina", "Kristof"]
let addresses: ThreeStringAddress = ["101 Betburweg", "66 Bellion Drive", "194 Laarderweg"]

type
    Matrix[W, H: static[int]] =
        array[1..W, array[1..H, int]]

let mat1: Matrix[2, 2] = [[1, 0],
                          [0, 1]]
let mat2: Matrix[2, 2] = [[0, 1],
                          [1, 0]]

proc `+`[W, H](a, b: Matrix[W, H]):
    Matrix[W, H] =
        for i in 1..high(a):
            for j in 1..high(a[0]):
                result[i][j] = a[i][j] + b[i][j]

# Seqs
var
    a = @[1, 2, 3]
    b = newSeq[int](3)

for i, v in a:
    b[i] = v*v

for i in 4..100:
    b.add(i * i)

b.delete(0)  # takes O(n) time
b = a[0] & b  # Same as original b

# JSON
import json

let element = "Hydrogen"
let atomicNumber = 1

let jsonObject = %* {"element": element, "atomicNumber": atomicNumber}
# This will print {"element":"Hydrogen", "atomicNumber": 1}
echo $jsonObject

# We start with a string representation of a JSON object
let jsonObject = """{"name": "Sky", "age": 32}"""
let jsonArray = """[7, 8, 9]"""

let parsedObject = parseJson(jsonObject)
let name = parsedObject["name"].getStr()
# This will print Sky
echo name

let parsedArray = parseJson(jsonArray)
let eight = parsedArray[1].getInt()
# This will print 8
echo eight

# First we'll define our types
type
    Element = object
        name: string
        atomicNumber: int

# Let's say this is the JSON we want to convert
let jsonObject = parseJson("""{"name": "Carbon", "atomicNumber": 6}""")

let element = to(jsonObject, Element)
# This will print Carbon
echo element.name
# This will print 6
echo element.atomicNumber

# Object Oriented Programming
type Animal = ref object of RootObj
    name: string
    age: int
method vocalize(this: Animal): string {.base.} = "..."
method ageHumanYrs(this: Animal): int {.base.} = this.age

type Dog = ref object of Animal
method vocalize(this: Dog): string = "woof"
method ageHumanYrs(this: Dog): int = this.age * 7

type Cat = ref object of Animal
method vocalize(this: Cat): string = "meow"

var animals: seq[Animal] = @[]
animals.add(Dog(name: "Sparky", age: 10))
animals.add(Cat(name: "Mitten", age: 10))

for a in animals:
    echo a.vocalize()
    echo a.ageHumanYrs()
