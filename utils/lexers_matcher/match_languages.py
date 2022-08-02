#!/usr/bin/env python3

"""
Copyright (c) 2022 Rafał Lalik <rafallalik@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

import argparse
import xml.etree.ElementTree as ET
import pygments.lexers as pyglex
import re
import yaml
from itertools import combinations
from colorama import Fore, Back, Style
#from fuzzywuzzy import fuzz

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-o', '--output', help='output file', type=str, default="lexers_found.xml")
    parser.add_argument('-v', '--verbose', help='verbose mode', action='store_true', default=False)
    opts, args = parser.parse_known_args()
    if opts.verbose:
        print("Options: ", opts, args)

    with open('config.yml', 'r') as file:
        config = yaml.safe_load(file)
    print(config)

    pygment_lexers = list(pyglex.get_all_lexers())

    kde_langs = []

    for xmlfile in args:
        if xmlfile == 'latex.xml':
            continue
        #print(f"Read {xmlfile}")
        rc = parseXML(xmlfile)
        if rc is not None:
            print(f"Found '{rc}' language syntax file")
            kde_langs.append(rc)

    if opts.verbose:
        print("***\nKDE languages: ", kde_langs)
        print("***\nPygments lexers: ", pygment_lexers, f"total = {len(pygment_lexers)}")
        print("\n***\nSearch for KDE lexer")

    matched_lexers = []
    minted_used_lexers = set()
    for kde_lang in kde_langs:
        lower_kl = kde_lang.lower()

        if kde_lang in config['minted_mapping']:
            val_of_minted_mapping = config['minted_mapping'][kde_lang]
            if opts.verbose:
                print(Fore.YELLOW + f"+ Mapped KDE lang '{kde_lang}' to minted lang '{val_of_minted_mapping}'" + Style.RESET_ALL)

            lexers = list(filter(lambda x: x[0] == val_of_minted_mapping, pygment_lexers))
            if len(lexers):
                #print(f"Lexers are {lexers[0][1]} from {pygment_lexers} and {kde_lang}")
                selected_lexers = filter_minted_lexers(lexers[0], config['minted_blacklisted'], minted_used_lexers)

                matched_lexers.append([kde_lang, val_of_minted_mapping, selected_lexers])
            else:
                if opts.verbose:
                    print(Fore.RED + f"No lexers from {pygment_lexers} and {kde_lang}" + Style.RESET_ALL)
            continue

        key_of_minted_matching = list(filter(lambda x: x[0].casefold() == kde_lang.casefold(), pygment_lexers))
        if len(key_of_minted_matching):
            if opts.verbose:
                print(Fore.GREEN + f"+ Matched KDE lang '{kde_lang}' to minted lang '{key_of_minted_matching[0][0]}'" + Style.RESET_ALL)

            selected_lexers = filter_minted_lexers(key_of_minted_matching[0], config['minted_blacklisted'], minted_used_lexers)

            matched_lexers.append([kde_lang, key_of_minted_matching[0][0], selected_lexers])
            continue

        #klm = max(kde_langs, key=lambda x: fuzz.ratio(ll[0].lower(), x.lower()))
        #klf = fuzz.ratio(ll[0].lower(), klm.lower())
        ##print(f"  Fuzz znalazł '{klm}'={klf} dla szukanego '{ll[1]}'")
        #if klf > int(config['fuzz_level']):
            #print(Fore.YELLOW + f"+ Fuzzed lexer '{ll[0]}' ({klf}) lang '{ll[1]}' with KDE lang '{klm}'" + Style.RESET_ALL)
            ##matched_lexers.append([ll[0], klm, ll[1]])
        #else:
        if opts.verbose:
            print(Fore.RED + f"- Lexer for KDE lang '{kde_lang}' not found" + Style.RESET_ALL)

    generate_output(matched_lexers, opts.output)


def generate_output(lexers, filename):
    print("\n***\nGenerate outputs")

    used_codes = []
    with open(filename, "w") as f:
        f.write('    <list name="MintedCodeLang">\n')
        last_lang = None

        for code,lang,ll in lexers:
            if last_lang is None or last_lang is not lang:
                f.write(f"      <!-- {code} lexers -->\n")

            for l in ll:
                f.write(f"      <item>{l}code*</item>\n")
                f.write(f"      <item>{l}code</item>\n")
        f.write('    </list>\n')

        used_langs = []
        mintenv_list = []
        f.write('\n\n\n\n\n')
        f.write('      <!-- environment type 5: minted environment created with newminted -->\n')
        f.write('      <context name="MintedCodeEnv" attribute="Environment" lineEndContext="#stay" fallthroughContext="#pop#pop#pop#pop">\n')
        for code,lang,ll in lexers:
            for l in ll:
                f.write(f'        <WordDetect String="{l}code*" attribute="Environment" context="Highlighting{code}CodeEnvS"/>\n');
                f.write(f'        <WordDetect String="{l}code" attribute="Environment" context="Highlighting{code}CodeEnv"/>\n');

            mintenv_list.append(f'        <WordDetect String="{l}" insensitive="true" context="Highlighting{code}CodeEnv"/>\n')

        f.write('''        <RegExpr String=".+code\*" attribute="Environment" context="UnknownHighlightingCodeEnvS"/>
        <RegExpr String=".+code" attribute="Environment" context="UnknownHighlightingCodeEnv"/>
      </context>\n\n''')


        f.write('      <context name="HighlightingSelector" attribute="Normal Text" lineEndContext="#stay">\n')
        f.write(''.join(mintenv_list))
        f.write('''        <AnyChar String="}]" context="#pop!UnknownHighlightingBegin"/>
        <RegExpr String="[^]}]*" context="#stay"/>
      </context>
''')
        f.write('\n')
        f.write('''      <context name="HighlightingCommon" attribute="Normal Text" lineEndContext="#stay">
        <RegExpr String="\\\\end\s*\{(?:lstlisting|minted|[a-zA-Z]+code)\*?\}" attribute="Structure" lookAhead="true" context="#pop#pop#pop#pop#pop#pop"/>
      </context>\n''')


        for code,lang,ll in lexers:
            f.write('''
      <context name="Highlighting{0}CodeEnvS" attribute="Error" lineEndContext="#stay">
        <DetectSpaces/>
        <DetectChar char="{2}" attribute="Normal Text" context="#pop!Highlighting{0}CodeEnvSParam"/>
      </context>
      <context name="Highlighting{0}CodeEnvSParam" attribute="Error" lineEndContext="#stay">
        <DetectSpaces attribute="Normal Text"/>
        <DetectChar char="{1}" attribute="Normal Text" context="Highlighting{0}CodeEnvSParamInside"/>
        <IncludeRules context="FindComments"/>
      </context>
      <context name="Highlighting{0}CodeEnvSParamInside" attribute="Normal Text" lineEndContext="#stay">
        <DetectSpaces/>
        <DetectIdentifier/>
        <DetectChar char="{2}" attribute="Normal Text" context="#pop!HighlightingBegin{0}"/>
        <IncludeRules context="FindComments"/>
        <RegExpr String="\&envname;" attribute="Macro" context="#stay"/>
      </context>
      <context name="Highlighting{0}CodeEnv" attribute="Normal Text" lineEndContext="#stay">
        <DetectChar char="{2}" context="HighlightingBegin{0}"/>
        <RegExpr String="[^{2}]*" attribute="Normal Text" context="#stay"/>
      </context>
      <context name="HighlightingBegin{0}" attribute="Normal Text" lineEndContext="#pop!Highlighting{0}">
        <DetectSpaces/>
        <RegExpr String=".+" attribute="Error" context="#stay"/>
      </context>
      <context name="Highlighting{0}" attribute="Normal Text" lineEndContext="#stay">
        <IncludeRules context="HighlightingCommon"/>
        <IncludeRules context="##{0}" includeAttrib="true"/>
      </context>
'''.format(code, '{', '}'))

        f.write('      <!-- end of mintedcode environment -->\n')


def filter_minted_lexers(lexers_list, blacklist, used_lexers):
    """Loop over lexers, filter out blacklisted and simplify names, pick up uniques"""
    selected = set()
    current_set = set()

    name_filter = r"^[^a-z]+|[^a-z]+$"

    for ll in lexers_list[1]:
        if ll in blacklist:
            print(f" Ignore blacklisted '{ll[1]}' lexer")
            continue

        new_ll = ll
        #rc = re.search(name_filter, new_ll)
        #new_ll = re.sub(r"[^a-z]", "", ll[0])
        new_ll = re.sub(r"\+\+", "pp", new_ll)
        new_ll = re.sub(r"\#", "sharp", new_ll)
        new_ll = re.sub(r"[^a-z]", "", new_ll)
        #new_ll = re.sub(r"[0-9]", "", new_ll)

        if new_ll != ll:
            print(f" {ll} => {new_ll}" + Fore.YELLOW + " - Replaced" + Style.RESET_ALL)

        rc = re.search(name_filter, new_ll)
        if rc is not None:
            print(f" {ll} => {new_ll}" + Fore.RED + " - Removed" + Style.RESET_ALL)
            continue

        if new_ll in used_lexers:
            print(f" {ll} => {new_ll}" + Fore.RED  + " - Ignored" + Style.RESET_ALL)
            continue

        print(f" {ll} => {new_ll}" + Fore.GREEN + " - Added" + Style.RESET_ALL)
        selected.add(new_ll)

    used_lexers.update(selected)    # update set of all lexers
    return sorted(selected)


def search_lexer(langname, pygment_lexers):
    for l in pygment_lexers:
        #print(l)
        if langname.lower() in (x.lower() for x in l[1]):
            return l[0]

    return None


def split_lexers(lexers):
    """Get pygments language with lexers and create all lexers list"""
    lexers_list = []
    for l in lexers:
        for ll in l[1]:
            lexers_list.append([ll, l[0], False])

    return lexers_list


def parseXML(xmlfile):
    tree = ET.parse(xmlfile)
    root = tree.getroot();
    if root.tag == 'language':
        langname = root.attrib['name']
        rc = root.find('highlighting')
        if rc is None:
            if opts.verbose:
                print(f"Language {langname} has no highlightng")
            return None

        return langname
    return None

if __name__ == '__main__':
    main()
