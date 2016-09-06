# Syntax Highlighting

Syntax highlighting engine for Kate syntax definitions

## Introduction

This is meant to be a stand-alone implementation of the Kate syntax highlighting engine,
focussed on a sufficiently complete feature coverage for read-only syntax highlighting.

Output "formats" include:

* QSyntaxHighlighter
* HTML


## Out of scope

This is not something to build an actual text editor with, as it doesn't cover any of the
features you want for this, such as:

* dynamic re-highlighting of changed areas
* code folding
* any of the not highlighting related properties in Kate's syntax definitions
* user interface for configuration
