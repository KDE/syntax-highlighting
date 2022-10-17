/* kate: hl CartoCSS
   This file contains some content coming from
   https://github.com/gravitystorm/openstreetmap-carto
   with CC0 license. This file is just for testing
   katepart highlighting engine.
   */

/* This is
a multiline comment. */

// This is a single-line comment.

#world {
    // various styles to define colors (all except the color function are rendered the same way):
    // Numbers are always treated as floating-point and highlighted. This syntax
    polygon-opacity: 50%;
    // is equivalent to
    polygon-opacity: 0.5;
    // This is a coma-separated list of numbers:
    line-dasharray: 6,3,2,3,2,3;
    // Also function calls are possible. Function calls like “url” get special highlighting:
    shield-file: url("myfile.svg");
    // Within function calls, strings can contains fields like [width] which get special highlighting:
    shield-file: url("symbols/shields/motorway_[width]x[height].svg");
    polygon-fill: white; // List of known colors
    polygon-fill: #ffffff; // Six-digit hex color
    polygon-fill: #fff; // Three-digit hex color
    polygon-fill: #ffff; // Invalid hex color
    polygon-fill: rgba(255,255,255,1); // define a color by a special function
    // everything else is simply interpreted as keyword:
    line-clip: false;
    line-clip: abcdef;
}

// You can define macros called “CartoCSS variables” with arbitrary values and types.
// They do not behave much like variable, but more like macros.
@myvariable: 15;
@myvariable: #123456;
@myvariable: white;
@myothervariable: @myvariable; // this variable gets defined by the value of another variable
@myvariable: darken(white, 5%); // A variable defined by the result of a function. The function “darken” gets special highlighting.
// Variables can also contain strings:
@myvariable: 'abc';

// These variables can be used later as values
#world {
    polygon-fill: @myvariable;
}

/* Typically, in MSS files you declare filters like “#world[zoom >= 17]”
followed by curly braces with parameters like “size” for Mapnik symbolizers like “text”. */

// Each Mapnik symbolizer parameter can be on its own line:
#world[zoom >= 17] {
  text-wrap-width: 20;
  text-size: 11;
}
// Or you can put various of them into the same line:
#world[zoom >= 19] { text-wrap-width: 20; text-size: 11; }

// It is allowed to omit the final “;” for the last Mapnik symbolizer parameter within a block:
#world[zoom >= 19] { text-wrap-width: 20; text-size: 11 }
// Also after an omitted final “;” the following code is nevertheless highlighted correctly:
#world[zoom >= 19] { text-wrap-width: 20; text-size: 11 ; }
// “null” and “zoom” are special keywords within filters, and highlighted as such:
["name" != null]["ref" = null][zoom >= 19] { text-wrap-width: 20; }

#admin-low-zoom[zoom < 11], // You can make coma-separated lists of various filters
#admin-mid-zoom[zoom >= 11][zoom < 13], // like this one.
#admin-high-zoom[zoom >= 13] {
  [zoom = 15] // “zoom” has special behaviour within filters and gets its own highlighting
  [admin_level = '2'], // data fields like “admin_level” can be referenced by their name and get highlighting as data fields
  ["admin_level" = '2'], // data fields like “admin_level” can be referenced by their name in quotes and get highlighting as data fields
  [admin_level = '3'] {
    [zoom >= 4] {
      // You can create additional “Named instances” of Mapnik symbolizers by adding a “myname/” before the symbolizer.
      // The name of the named instance gets special highlighting:
      background/line-color: white;
      line-color: @admin-boundaries;
    }
  }
}

#admin-mid-zoom[zoom >= 11][zoom < 13] {
  [admin_level = 'abc'] { // The string 'abc' is highlighted as a verbatim string, not as an expression string.
    [zoom >= 4] {
      text-name: [test]; // simplified reference to the value of the data field “test”
      text-name: "[test]"; // another reference to the data field “test”, this time within an expression string
      // A rather complex expression string that will do some math; @zoom is a special runtime value (in spite
      // of the @ it has nothing to do with ordinary variables):
      text-size: "([way_area]*pow(4,@zoom)/24505740000)";
      // Here the same thing as simple expression:
      // Note that @zoom now is highlighted in another color, because in this context it does not reference to a
      // runtime variable anymore, but to an ordinary CartoCSS variable:
      text-size: ([way_area]*pow(4,@zoom)/24505740000);
      text-name: "'Value: '[test]"; // A verbatim string 'Value: ' as part of an expression string.
      text-name: '"Value: "[test]'; // " and ' are interchangeable. The outer is always the expression string and the inner the verbatim string.
      text-name: "[field]\n"; // Expression strings however do not have escape sequences, so \n gets no special highlighting here. [field] however is highlighted as data field.
      text-name: "[field]+'\n'+[otherfield]"; // Inner strings, here the single-quoted string, are normal strings and highlight escape sequences like \n.
      text-name: "[field]+'[testfield]'+[otherfield]"; // Inner strings, here the single-quoted string, are normal strings and do not highlight any fields like [testfield].
      text-name: [name] + /* Comments are not allowed within expressions, so no special comment highlighting here. */ "\n" + [ref];
    }
  }
}

.nature { // .nature references a class, which is similar to a layer like #nature, so both are rendered the same way
    ::fill {
        /* The :: syntax defined “attachments” (a sort of sub-layer within normal layers), here “::fill”.
        Everything that is defined within an attachment is applied in the order of the first occurrence
        of the attachment, instead of following the normal appliance order. Therefore attachments are
        highlighted as dsControlFlow by default. */
        opacity: 0.05;
    }
}
