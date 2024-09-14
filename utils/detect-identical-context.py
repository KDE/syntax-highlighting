#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2023 Jonathan Poelen <jonathan.poelen@gmail.com>
# SPDX-License-Identifier: MIT

from typing import Iterable, Mapping
from xml.etree.ElementTree import ElementTree

import sys


def print_usage_and_exit():
    print(sys.argv[0], '[-p] syntax.xml...\n  -p  show duplicate content', file=sys.stderr)
    exit(1)

def normalize_bool_or_remove_if_false(d: dict[str, str], key: str) -> None:
    value = d.get(key)
    if value is not None:
        if value == '1' or value.lower() == 'true':
            d[key] = '1'
        else:
            d.pop(key)

def remove_if_stay(d: dict[str, str], key: str) -> None:
    value = d.get(key)
    if value is not None and (not value or value == '#stay'):
        d.pop(key)


if len(sys.argv) < 2 or sys.argv[1] in {'-h', '--help'}:
    print_usage_and_exit()

iarg = 1
show_content = sys.argv[1] == '-p'

if show_content:
    iarg += 1
    if len(sys.argv) < 3:
        print_usage_and_exit()

for filename in sys.argv[iarg:]:
    tree = ElementTree()
    tree.parse(filename)

    identical_contexts = {}

    for context in tree.getroot()[0].find("contexts"):
        if len(context) == 0 or (len(context) == 1 and context[0].tag == 'IncludeRules'):
            attrib = context.attrib
            name = attrib.pop('name')
            remove_if_stay(attrib, 'lineEndContext')
            remove_if_stay(attrib, 'lineEmptyContext')
            remove_if_stay(attrib, 'fallthroughContext')
            if len(context):
                attrib.update(context[0].attrib)
                normalize_bool_or_remove_if_false(attrib, 'includeAttrib')

            s = '\x01'.join(sorted(f'{k}={v}' for k,v in attrib.items()))
            identical_contexts.setdefault(s, []).append(name)
        else:
            rules = set()
            for rule in context:
                attrib = rule.attrib
                remove_if_stay(attrib, 'context')
                normalize_bool_or_remove_if_false(attrib, 'dynamic')
                normalize_bool_or_remove_if_false(attrib, 'minimal')
                normalize_bool_or_remove_if_false(attrib, 'includeAttrib')
                normalize_bool_or_remove_if_false(attrib, 'firstNonSpace')
                normalize_bool_or_remove_if_false(attrib, 'lookAhead')
                s = '\x01'.join(f'{k}={v}' for k,v in sorted(attrib.items()))
                rules.add(f'{rule.tag}\x01{s}')

            identical_contexts.setdefault('\n'.join(sorted(rules)), []).append(context.attrib['name'])

    for content, names in identical_contexts.items():
        if len(names) > 1:
            print(f'{filename}: {names}')
            if show_content:
                print(' ', content.replace('\x01', ' ').replace('\n', '\n  '))
