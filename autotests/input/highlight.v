// Comment
/**
 * comment 2
 */
fn main() {
	mut s := ''
	mut str := 'asd\n'
	mut str := 'asd \' asd'
	mut str := "asd \" asd"
	mut ch := `a`
	mut n := 1.2
	mut d := 12
	for n in 1 .. 101 {
		if n % 3 == 0 {
			s += 'Fizz'
		}
		if n % 5 == 0 {
			s += 'Buzz'
		}
		if s == '' {
			println(n)
		} else {
			println(s)
		}
		s = ''
	}
}
