/*Basic test file
Written by Julian Stirling, 2018
License: MIT*/

// This is a comment
x=5;
y=6;
z=5;
r=2;
for (n = [-1.5:1:1.5]){
    translate([n*x,0,0]){cubehole([x,y,z],r);}
}

module cubehole(size,holerad)
{
    $fn=28;
    difference()
    {
        cube(size,center=true);
        cylinder(size[2]+1,r=holerad,center=true);
    }
}

// some examples adapted from https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/General
cube(5);
x = 4 + y;
rotate(40) square(5,10);
translate([10, 5]) {
    circle(5);
    square(4);
}
rotate(60) color("red") {
    circle(5);
    square(4);
}
color("blue") {
    translate([5, 3, 0]) sphere(5);
    rotate([45, 0, 45]) {
        cylinder(10);
        cube([5, 6, 7]);
    }
}

aNumber = 42;
aBoolean = true;
anotherBoolean = false;
aString = "foo";
aRange = [0: 1: 10];
aVector = [1, 2, 3];
aUndef = undef;
moreNumbers = [-1, 42, 0.5, 2.99792458e+8, 1.337e3, 314e-2];
echo(moreNumbers)
echo("The quick brown fox \tjumps \"over\" the lazy dog.\rThe quick brown fox.\nThe \\lazy\\ dog. \x21 \u03a9 \U01F98A \U01f43e");

vector1 = [1,2,3]; vector2 = [4]; vector3 = [5,6];
new_vector = concat(vector1, vector2, vector3); // [1,2,3,4,5,6]

string_vector = concat("abc","def");                 // ["abc", "def"]
one_string = str(string_vector[0],string_vector[1]); // "abcdef"

a = [1,2,3]; echo(len(a));   //  3

// Example which defines a 2D rotation matrix
mr = [
    [cos(angle), -sin(angle)],
    [sin(angle),  cos(angle)]
];

// modifiers
% cube([10, 10, 10]);  // background
# cube([10, 10, 10]);  // debug
! cube([10, 10, 10]);  // root
* cube([10, 10, 10]);  // disable

// if example taken from https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/Other_Language_Features#Rounding_examples
if(a==b){
    echo ("a==b");
}else if(a>b){
    echo ("a>b");
}else if(a<b){
    echo ("a<b");
}else{
    echo ("???");
}

// assert example taken from https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/Other_Language_Features#assert
module row(cnt = 3){
    // Count has to be a positive integer greater 0
    assert(cnt > 0);
    for (i = [1 : cnt]) {
        translate([i * 2, 0, 0]) sphere();
    }
}
row(0);
