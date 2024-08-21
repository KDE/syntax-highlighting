# Sample Nix file
# ---------------
let
    bool = true && false;
    var1 = if 3 < 4 then "a" else "b";
in {
    /*
       Multi-line comments
    */
    inherit var1;

    var2 = with builtins; [
        0 1 2 (-3) (-4)
        123.456 .12e34
        (12 + 345 * 6789 / 321)
    ];

    var3 = [
        "Single-line string"
        "
          Multi-line string
        "
        ''
          Indented string.
        ''
    ];
    nested.var4 = [
        "String with ${var1}"
        "Another one with escaped \${var1}"
        ''
          Document with ${var1}
          which is ''${var1}
        ''
        ''
          My home: ${builtins.getEnv "HOME"}
          Escaped tab: ''\\t
        ''
    ];

    nested.a.imported = (import ./example.nix);

    fn = (x: y: x + y);

    fn1 = { a, b ? import ./file.nix, c ? { a = 1; b = 2; }, ... }: rec {
        inherit (import ./.);
        x = a;
    };

    var5 = {
        inherit a b c;
        inherit (expr) a b c;
    };

    var6 = {
        ${foo} = bar;
        ${bar} = baz;
        a.${b} = c;
    };

    var7 = {
        "first" = 1;
        "second" = 2;
    };

    abc."cde".fgh = abc."cde".fgh;
}
