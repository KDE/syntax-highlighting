<?
/* This is a pseudo PHP file to test Kate's PHP syntax highlighting. */
# TODO: this is incomplete, add more syntax examples!
# this is also a comment.
// Even this is a comment
/** \brief doxygen comment
*/ /*normal comment*/
// comment ?> normal text <?php
function test($varname) {
	return "bla";	# this is also a comment
}
?>

<?php echo("hello test"); ?>

<html>
	<? print "<title>test</title>"; ?>

	<!-- CSS -->
	<style>
		.inputText {
			width: <?php echo $width; ?>px;
			text-indent: 10px;
		}
	</style>

	<!-- JavaScript -->
	<script>
		var some_js_var = <?php echo $somevar; ?> ;
		<?php echo 'alert("Hello there.");'; ?>
	</script>
	<!-- JavaScript React -->
	<script type="text/babel">
		<Hello> <?php echo("Hello, hello!"); ?> </Hello> <?php /* aaa */ ?> <div></div>
		function a(i) {
			<?php echo "var j = 1;"; ?>
			return <p>{ i + j }</p>;
		}
	</script>
	<!-- TypeScript -->
	<script type="text/typescript">
		<?php $timestamp = time(); ?>
		class DateTime {
			info: string;
			constructor() { this.info = <?php echo(date("F d, Y h:i:s", $timestamp)); ?>; }
			get() { return this.info; }
		}
	</script>
	<!-- MustacheJS -->
	<script type="x-tmpl-mustache">
		{{! <?php print "comment"; ?> }}
		{{#movie}}
			<div>
				<h1>{{title}}</h1>
				<img src="{{poster}}" alt="{{title}}"/>
				<?php echo $movierating; ?> - {{ratings.critics_rating}}
			</div>
		{{/movie}}
	</script>

</html>

<?php
$var = <<<DOOH
This is the $string inside the variable (which seems to be rendered as a string)
It works well, I think.
DOOH

# bug 382527
throw new ParserException("Test {$this->some_var[$index]}\nin {$this->file} is missing.\nThis is bad.");

$arr = array();
foreach($arr as $i => $v):
	echo($i);
endforeach

echo " $var {$var} ${var} ";
echo " $var[1] {$var[1]} ${var[1]} ";
echo " $var[-1] {$var[-1]} ${var[-1]} ";
echo " $var[i] {$var[i]} ${var[i]} ";
echo " $var[-i] {$var[-i]} ${var[-i]} ";
echo " $var[$i] {$var[$i]} ${var[$i]} ";
echo " $var[-$i] {$var[-$i]} ${var[-$i]} ";
echo " $var[$$i] {$var[$$i]} ${var[$$i]} ";
echo " $var[$x->i] {$var[$x->i]} ${var[$x->i]} ";
echo " $var[$x->$i] {$var[$x->$i]} ${var[$x->$i]} ";
echo " $$var {$$var} $${var} ";
echo " $$var[1] {$$var[1]} $${var[1]} ";
echo " $$var[$i] {$$var[$i]} $${var[$i]} ";
echo " $var[$i] {$var[$i]} ${var[$i]} ";
echo " $var[1+1] {$var[1+1]} ${var[1+1]} ";
echo " $var->mem {$var->mem} ${var->mem} ";
echo " $var->3 {$var->3} ${var->3} ";
echo " $var-> {$var->} ${var->} ";
echo " $var->$mem {$var->$mem} ${var->$mem} ";
echo " ${$x->{'mem'}} ";
echo "\n \t \\ \xff \x";

echo <<<HTML
<p>bla $var \$var bla\'</p>
HTML;
echo <<<"HTML"
<p>bla $var \$var bla\'</p>
HTML;
echo <<<'HTML'
<p>bla $var \$var bla\'</p>
HTML;
echo <<<DOC
<p>bla $var \$var bla\'</p>
 DOC
 DOC;
DOC;blah
DOC;

echo 123456 1_2_34_5 1__2 1_;
echo 0x12345af 0x1_2_3_45_a_f 0x1__2 0x_1 0x1_ 0x1g 0xg;
echo 0b1011 0b1_01_1 0b1__0 0b_1 0b1_ 0b12 0b2;
echo 01234 01_23_4 0_1 0__1 01_ 018 08;
echo 0o1234 0O1_23_4 0o1_1_ 0o1__1 0o_1;
echo 123. .45 123.45 1_23.4_5e1_2;
echo 123.e1 .45e1 123.45e1;
echo 123.e+1 .45e+1 123.45e+1;
echo 123.e++1 ._45e+1 123._45e+1; // bad float

echo 123...45;
echo add(...[1, 2])."\n";

function gen_three_nulls() {
    foreach (range(1, 3) as $i) {
        yield;
    }
}

function &gen_reference() {
    $value = 3;

    while ($value > 0) {
        yield $value;
    }
}

function gen() {
    yield 0; // key 0
    yield from inner(); // keys 0-2
    yield 4; // key 1
}

switch (1) {
    case 1: echo 'plop';
    case 1; echo 'plop';
    default: echo 'plop';
    default : echo 'plop';
}

class User
{
    public int $id;
    public ?string $name;

    public function __construct(int $id, ?string $name)  : void
    {
        $this->id = $id;
        $this->name = $name;
    }
}

class MyClass {
    const CONST_VALUE = 'A constant value';
}

class OtherClass extends MyClass
{
    public static $my_static = 'static var';

    public static function doubleColon() {
        echo parent::CONST_VALUE . "\n";
        echo self::$my_static . "\n";
    }
}

$classname = 'MyClass';
echo $classname::CONST_VALUE;

echo MyClass::CONST_VALUE;

$classname = 'OtherClass';
$classname::doubleColon();

OtherClass::doubleColon();
new $classname::class;

$user = new User(1234, null);

$y = 1;
$fn1 = fn($x) => $x + $y;
$fn2 = function ($x) use ($y) {
    return $x + $y;
};

$a = [1,2,3,'x'=>4];
$b = array(1,2,3,'x'=>4);

namespace A\B\C;
class Exception extends \Exception {}

$b = new \Exception('hi');
$x = new A\B\Foo('hi');
$n = \strlen($str) - 1;

$r = $x <=> $y and $z;

#[ExampleAttribute('Hello world', 42)]
class Foo {}
function foo(#[TestAttr] $bar){
}

enum Suit {
    case Clubs;
    case Diamonds;
}
?>

<?php if (n): ?>
<p>bla ba</p>
<?php endif ?>
