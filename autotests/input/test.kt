package org.example

fun main(args: Array<String>) {
    val sum1 = add(5, 6)
    println(sum1)
    println("""
$$a${1+2}\$$
abc
""")
}
 
/**
* add function with two, three or four integer arguments
* c : default argument is 0
* d : default argument is 0
*/
fun add(a:Int, b:Int, c:Int = 0, d:Float = 12e10F): Int {
    return a + b + c + d
} 

public class MyTest {
    lateinit var subject: TestSubject

    val dyn: dynamic

    @get:Rule val tempFolder = TemporaryFolder()

    @set:[Inject VisibleForTesting]
    var collaborator: Collaborator

    @SetUp fun setup() {
        subject = TestSubject()
    }

    fun @receiver:Fancy String.myExtension() {

    }

    @Test fun test() {
        subject.method()  // dereference directly
    }
}

import /* comment */ foo.Foo // comment
class /* comment */ Foo {
    fun /* comment */ foo(/* comment */) {

    }

    val /* comment */ bar = 1
    var /* comment */ baz = 2
}
