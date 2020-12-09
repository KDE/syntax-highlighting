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
