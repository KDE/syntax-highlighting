# Minted lexer matcher

Purpose of this tool is to make matchin between `minted` availabl lexers and syntax schemas provided by `syntax-highlighter`.

### Usage

Call script with xml files as arguments. The `latex.xml` in the input will be ignored.

```bash
match_languages.py ../../data/syntax/*.xml
```

The script will generate an output file `lexers_found.xml` which should be manually merged into `../../data/syntax/latex.xml`, e.g.

```bash
meld lexers_found.xml ../../data/syntax/latex.xml
```

Additional options to the script:

* `-v` - more verbose output
* `-o file_name` - output file name

### Configuration

`config.yml` provides extra configuration for the tool.

* `fuzz_level` - used for fuzzy matching, not used anymore
* `minted_blacklisted` - list of languages in minted, which should be ignored (e.g. due to duplication or latex incompatible names, like 'c++' and 'cpp' in minted, only 'cpp' will work with `mycode`
* `minted_mapping` - list of `key: val` pairs, where `key` is minted name of language, anbd `val` is corresponfing lexer in this repository, for all pairs which do not have exact matching
* `lstlisting_mapping` - same as for minted but for lstlistings, currenlty not supported yet.

Whenever `minted` language name is references in fact it refeers to `pygments` which provides syntax highlighting for `minted`.

## LICENSE

The script is distributed under MIT license.