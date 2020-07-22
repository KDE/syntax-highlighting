"""module docstring"""

namespace My.NameSpace #optional namespace declaration

import Assembly.Reference #import statements

#followed by the Members of this module (classes, methods, etc.)
class MyClass:
    pass

def domyfunction(it):
    print(it)

#start "main" section that is executed when script is run
x as int
x = 3
domyfunction(x)

#optional assembly attribute declarations used when compiling
[assembly: AssemblyTitle('foo')]
[assembly: AssemblyDescription('bar')]

import MyLibrary
print (Version)
doit()

[Module]
class MainClass:
    public static Version as string

    static def constructor():
        Version = "0.1"

def doit():
    #you can refer to "globals" from within your library, too:
    print("This library's version is: "+MainClass.Version)

