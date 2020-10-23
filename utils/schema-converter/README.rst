What is This
============

The new kate's theme format incompatible with the previously used ``*.kateschema``
and ``*.katehlcolor`` files. Here is a tool aimed to help convert old colors to
the newly used theme files.


Usage
=====

- Make a virtualenv with all dependencies::

    $ pipenv install --dev

- Enter the virtualenv::

    $ pipenv shell

- Check the help screen::

    Usage: kateschema2theme [OPTIONS] [INPUT_FILE]

    Options:
    -h, --help             Show this message and exit.
    --version              Show the version and exit.
    -d, --skip-included    Do not write custom colors included from another
                           syntax files.

    -s, --syntax-dir TEXT  Specify the directory to search for syntax files. If
                           given, extra validation going to happen.

- Example *katecolor* file::

    $ cat .desktop.katehlcolor
    [Highlighting .desktop - Schema Zaufi::dark]
    .desktop:Comment=8,ff787775,ff787775,0,1,0,0,,,,---
    .desktop:Key=2,ff77aa77,ff77aa77,0,0,0,0,,,,---
    .desktop:Language=3,ff4086c0,ff4086c0,1,0,0,0,,,,---
    .desktop:Normal Text=0,ffcacaca,ffcacaca,0,0,0,0,,,,---
    .desktop:Section=1,ffdaaa66,ffdaaa66,1,0,0,0,,,,---

    [KateHLColors]
    full schema=false
    highlight=.desktop
    schema=Zaufi::dark

- Perform the conversion::

    $ kateschema2theme .desktop.katehlcolor
    {
        "custom-styles": {
            ".desktop": {
                "Comment": {
                    "bold": false,
                    "italic": true,
                    "selected-text-color": "#787775",
                    "strike-through": false,
                    "text-color": "#787775",
                    "underline": false
                },
                "Key": {
                    "bold": false,
                    "italic": false,
                    "selected-text-color": "#77aa77",
                    "strike-through": false,
                    "text-color": "#77aa77",
                    "underline": false
                },
                "Language": {
                    "bold": true,
                    "italic": false,
                    "selected-text-color": "#4086c0",
                    "strike-through": false,
                    "text-color": "#4086c0",
                    "underline": false
                },
                "Normal Text": {
                    "bold": false,
                    "italic": false,
                    "selected-text-color": "#cacaca",
                    "strike-through": false,
                    "text-color": "#cacaca",
                    "underline": false
                },
                "Section": {
                    "bold": true,
                    "italic": false,
                    "selected-text-color": "#daaa66",
                    "strike-through": false,
                    "text-color": "#daaa66",
                    "underline": false
                }
            }
        },
        "metadata": {
            "name": "Zaufi::dark",
            "revision": 1
        }
    }

- Because the syntax files also may be changed, and some syntax items could
  disappear, or some new added. The old theme files may contain unused entries
  as well as missed definitions for the newly added items. To address these
  possible issues, the ``-s`` option could be used with a path to the syntax
  XMLs directory. The conversion tool will validate the items defined in
  the old there against actually declared by the corresponding syntax.
  Unused items will be discarded from the converted theme::

    $ kateschema2theme -s /usr/share/org.kde.syntax-highlighting/syntax very-old.kateschema >/dev/null
    …
    * The following styles are not used by `CMake` syntax anymore:
      Commands               │ CMake Variable       │ Environment Variable
      Builtin CMake Variable │ Properties
      Macros                 │ Third-Party Commands

    * The following styles are not defined in the converted `CMake` syntax:
      User Function/Macro   │ False Special Arg                 │ Command
      Named Args            │ Cache Variable Substitution
      Internal Name         │ CMake Internal Variable
      Variable Substitution │ Property
      True Special Arg      │ Environment Variable Substitution
      Builtin Variable      │ Standard Environment Variable
      Aliased Targets       │ @Variable Substitution
    …
