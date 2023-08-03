local utils = import 'utils.libsonnet';

/* A C-style
   comment.
*/
# A Python-style comment.

// Define a "local" function.
// Default arguments are like Python:
local my_function(x, y=10) = x + y;

local object = {
  // A method
  my_method(x): x * x,
};

local string1 = 'Farmer\'s Gin';
local string_newline1 = 'this is
a valid string
with newlines';
local string_newline2 = "this is
also
a valid string";

local timcollins = |||
 The Tom Collins is essentially gin and
 lemonade. The bitters add complexity.
|||;

local obj = {
  "foo": 1,
  "bar": {
    "arr": [1, 2, 3],
    "number": 10 + 7,
  },
};

// Define a local function.
// Default arguments are like Python:
local my_function(x, y=10) = x + y;

// Define a local multiline function.
local multiline_function(x) =
  // One can nest locals.
  local temp = x * 2;
  // Every local ends with a semi-colon.
  [temp, temp + 1];

local object = {
  // A method
  my_method(x): x * x,
};

local large = true;

{

  // Functions are first class citizens.
  call_inline_function:
    (function(x) x * x)(5),

  call_multiline_function: multiline_function(4),

  // Using the variable fetches the function,
  // the parens call the function.
  call: my_function(2),

  // Like python, parameters can be named at
  // call time.
  named_params: my_function(x=2),
  // This allows changing their order
  named_params2: my_function(y=3, x=2),

  // object.my_method returns the function,
  // which is then called like any other.
  call_method1: object.my_method(3),

  standard_lib:
    std.join(' ', std.split("foo/bar", '/')),
  len: [
    std.length('hello'),
    std.length([1, 2, 3]),
  ],

  cocktails: {
    local factor = if large then 2 else 1,

    // Ingredient quantities are in fl oz.
    'Tom Collins': {
      ingredients: [
        { kind: string1, qty: 1.5*factor },
        { kind: 'Lemon', qty: 1 },
        { kind: 'Simple Syrup', qty: 0.5E+1 },
        { kind: 'Soda', qty: 2 },
        { kind: 'Angostura', qty: 'dash' },
      ],
      garnish: 'Maraschino Cherry',
      served: 'Tall',
      description: timcollins,
    },
  },
}
