#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2023 Jonathan Poelen <jonathan.poelen@gmail.com>
# SPDX-License-Identifier: MIT

from argparse import ArgumentParser
from pathlib import Path
from typing import Iterable, Mapping
from textwrap import wrap
from xml.parsers import expat

import re


parser = ArgumentParser(
    prog='generate-dot-file.py',
    description=f'''Dot file generator for xml syntax

Example:
    generate-dot-file.py data/syntax/lua.xml | dot -T svg -o image.svg && xdg-open image.svg''')

parser.add_argument('-c', '--context-only', action='store_true',
                    help='Generates contexts without rules')

parser.add_argument('-r', '--resolve-entities', action='store_true',
                    help='Evaluate xml entities')

parser.add_argument('-i', '--include', action='append', default=[],
                    help='Include only contexts that respect a pattern')

parser.add_argument('-e', '--exclude', action='append', default=[],
                    help='Exclude contexts that respect a pattern')

parser.add_argument('syntax', help='XML Syntax Definition Files')


args = parser.parse_args()

excludes = [re.compile(patt) for patt in args.exclude]
includes = [re.compile(patt) for patt in args.include]
context_only = args.context_only
resolve_entities = args.resolve_entities or context_only


global_entities = {
    '&#9;': '\\t',
    '&#37;': '%',
    '&#38;': '&',
    '&amp;': '&',
    '&#39;': "'",
    '&aquot;': "'",
    '&#34;': '"',
    '&quot;': '"',
    '&#60;': '<',
    '&lt;': '<',
    '&#62;': '>',
    '&gt;': '>',
}
entities_finder = re.compile('|'.join(global_entities))



Outside = 0
Context = 1
Rule = 2

class XMLParser:
    depth = Outside
    matched = False
    ictx = 0
    ctx_name = ''
    ctx_attrs: dict[str, str] = {}
    escaped_ctx_name = ''
    ctx_color = ''
    irule = 0

    resolve_entities = True
    reversed_entities: dict[str, str] = {}
    resolved_entity_searcher: re.Pattern

    def __init__(self, start_ctx, end_ctx, rule_process):
        self.start_ctx = start_ctx
        self.end_ctx = end_ctx
        self.rule_process = rule_process

    def start_element(self, tag: str, attrs: dict[str, str]):
        if self.depth == Context:
            self.depth = Rule
            if self.matched:
                self.irule += 1
                if not self.resolve_entities:
                    string = attrs.get('String')
                    if string:
                        attrs['String'] = self.unresolve_entities(string)
                self.rule_process(self, self.irule, tag, attrs)
        elif tag == 'context':
            name = attrs['name']
            self.depth = Context
            self.matched = (not match_patterns(name, excludes)
                            and (not includes or match_patterns(name, includes)))
            if self.matched:
                self.irule = 0
                self.ctx_name = name
                self.ctx_attrs = attrs
                self.escaped_ctx_name = escape(name)
                self.ctx_color = compute_color(name)
                self.start_ctx(self)

    def end_element(self, name: str):
        if self.depth == Context:
            if self.matched:
                self.end_ctx(self)
            self.ictx += 1
        self.depth -= 1

    def unresolve_entities(self, s: str) -> str:
        """
        expat module converts all entities. This function tries to do the
        opposite by replacing pieces of text with entities.

        The result may differ from the original text, but will be equivalent.
        """
        b = True
        def replace(m):
            nonlocal b
            b = True
            return self.reversed_entities[m[0]]
        while b:
            b = False
            s = self.resolved_entity_searcher.sub(replace, s)
        return s

    def entity_decl(self, name, is_parameter_entity, value, base, system_id, public_id, notation_name):
        value = entities_finder.sub(lambda m: global_entities[m[0]], value)
        self.reversed_entities[value] = f'&{name};'

    def end_doctype(self):
        patt = '|'.join(re.escape(value) for value in self.reversed_entities)
        self.resolved_entity_searcher = re.compile(patt)


color_map = [
    '"/rdgy4/3"',
    '"/set312/1"',
    '"lightgoldenrod1"',
    '"/set312/3"',
    '"/set312/4"',
    '"/set312/5"',
    '"/set312/6"',
    '"/set312/7"',
    '"/rdpu3/2"',
    '"/purd6/3"',
    '"/ylgn4/2"',
    '"/set26/6"',
]

picked_colors: dict[int, str] = {}

def compute_color(name: str) -> str:
    """
    returns a color which depends on the first 2 characters
    """
    k = ord(name[0])
    if len(name) > 1:
        k += ord(name[1]) * 1024
    color = color_map[len(picked_colors) % len(color_map)]
    return picked_colors.setdefault(k, color)


def match_patterns(name: str, patterns: list[re.Pattern]) -> bool:
    return any(patt.search(name) for patt in patterns)


_pop_counter_re = re.compile('^(?:#pop)+')

def labelize(name: str) -> str:
    m = _pop_counter_re.match(name)
    if m:
        n = len(m[0]) // 4
        if n > 1:
            return f'#pop({n}){name[n * 4:]}'
    return name


def stringify_attrs(attr_names: Iterable[str], attrs: Mapping[str, str]) -> str:
    s = ''
    for name in attr_names:
        attr = attrs.get(name)
        if attr:
            part = '\n'.join(wrap(attr, 40))
            s += f'  {v}:{part}'
    return s


def escape(s: str) -> str:
    return s.replace('\\', '\\\\').replace('"', '\\"')


def jumpctx(s: str) -> str:
    i = s.find('!')
    return '' if i == -1 else s[i+1:]


def xml_bool(s: str | None) -> bool:
    return s == '1' or s == 'true'


def push_context_attr(output: list[str],
                      escaped_origin: str, escaped_ctx_name: str, escaped_name_attr: str,
                      style: str, color: str) -> None:
    if escaped_name_attr == '#stay':
        output.append(f'    "{escaped_origin}" -> "{escaped_ctx_name}" [style={style},color={color}];\n')
    elif escaped_name_attr.startswith('#'):
        ref = f'{escaped_ctx_name}!!{escaped_name_attr}'
        output.append(
            f'    "{escaped_origin}" -> "{ref}" [style={style},color={color}];\n'
            f'    "{ref}" [label="{labelize(escaped_name_attr)}",color={color}];\n'
        )


def push_last_transition(output: list[str],
                         escaped_name: str, escaped_ctx_name: str, escaped_name_attr: str,
                         color: str) -> None:
    if escaped_name_attr == '#stay':
        return

    if escaped_name_attr.startswith('#'):
        escaped_last_ctx = jumpctx(escaped_name_attr)
        if escaped_last_ctx:
            output.append(f'  "{escaped_ctx_name}!!{escaped_name_attr}" -> "{escaped_last_ctx}" [style=dashed,color={color}];\n')
    else:
        output.append(f'  "{escaped_name}" -> "{escaped_name_attr}" [style=dashed,color={color}];\n')


output = [
    'digraph G {\n',
    '  compound=true;ratio=auto\n'
]

if context_only:
    # avoid multi arrow for ctx1 -> ctx2
    krule_contexts: dict[str, int] = {}
    # shares #pop... nodes
    kpoped_contexts: dict[tuple[str, str], str] = {}

    def start_ctx(p: XMLParser):
        krule_contexts.clear()

    def rule_process(p: XMLParser, irule: int, name: str, attrs: dict[str, str]):
        krule_contexts[attrs.get('context') or '#stay'] = irule

    def end_ctx(p: XMLParser):
        color = p.ctx_color
        ctx_name = p.escaped_ctx_name
        output.append(f'  "{ctx_name}" [style=filled,color={color}]\n')

        krule_contexts.setdefault(p.ctx_attrs.get('fallthroughContext') or '#stay', -1)
        krule_contexts.setdefault(p.ctx_attrs.get('lineEndContext') or '#stay', -2)
        krule_contexts.setdefault(p.ctx_attrs.get('lineEmptyContext') or '#stay', -3)

        krule_contexts.pop('#stay')

        for rule_context, i in sorted(krule_contexts.items(), key=lambda t: t[1]):
            if i >= 0:
                style = f'color={color}'
            elif i == -1:
                style = f'style=dashed,color={color}'
            elif i == -2:
                style = 'style=dotted,color=blue'
            else:  # if i == -3:
                style = 'style=dotted,color=purple'

            escaped_rule_context = escape(rule_context)
            labelized_context = labelize(escaped_rule_context)
            if rule_context.startswith('#'):
                next_context = jumpctx(escaped_rule_context)
                if next_context:
                    k = (labelized_context, next_context)
                    poped_context = kpoped_contexts.get(k)
                    if poped_context:
                        output.append(f'  "{ctx_name}" -> "{poped_context}" [{style}];\n')
                    else:
                        poped_context = f'{ctx_name}!!{i}'
                        kpoped_contexts[k] = poped_context
                        output.append(f'  "{ctx_name}" -> "{poped_context}" [{style}];\n'
                                      f'  "{poped_context}" [label="{labelized_context}"];\n'
                                      f'  "{poped_context}" -> "{next_context}"\n')
                else:
                    poped_context = f'{ctx_name}!!{i}'
                    output.append(f'  "{ctx_name}" -> "{poped_context}" [{style}];\n'
                                  f'  "{poped_context}" [label="{labelized_context}"];\n')
            else:
                output.append(f'  "{ctx_name}" -> "{labelized_context}" [{style}]\n')

else:
    first_line_attributes = ('attribute', 'String', 'char')  # char1 is tranformed into String
    second_line_attributes = ('beginRegion', 'endRegion', 'lookAhead', 'firstNonSpace', 'column', 'additionalDeliminator', 'weakDeliminator')

    kdot: dict[str, tuple[str, int]] = {}
    escaped_name = ''

    def start_ctx(p: XMLParser):
        global escaped_name

        escaped_name = p.escaped_ctx_name

        kdot.clear()
        output.append(
            f'  subgraph cluster{p.ictx} {{\n'
            f'    "{escaped_name}" [shape=box,style=filled,color={p.ctx_color}];\n'
        )

    def rule_process(p: XMLParser, irule: int, name: str, attrs: dict[str, str]):
        global escaped_name

        color = p.ctx_color
        escaped_ctx_name = p.escaped_ctx_name

        next_name = f'{p.ctx_name}!!{irule}!!{name}'
        escaped_next_name = escape(next_name)
        rule_context = attrs.get('context', '#stay')
        output.append(f'    "{escaped_name}" -> "{escaped_next_name}" [style=dashed,color={color}];\n')

        escaped_name = escaped_next_name

        if name == 'IncludeRules':
            label = f'  {rule_context}'
        else:
            if 'attribute' not in attrs:
                attrs['attribute'] = p.ctx_attrs['attribute']
            if 'char1' in attrs:
                attrs['String'] = attrs.pop('char') + attrs.pop('char1')
            label = stringify_attrs(first_line_attributes, attrs)
            label2 = stringify_attrs(second_line_attributes, attrs)
            if label2:
                label = f'{label}\n{label2}'
        output.append(f'    "{escaped_name}" [label="{name}{escape(label)}"];\n')

        if xml_bool(attrs.get('lookAhead')):
            output.append(f'    "{escaped_name}" [style=dashed];\n')

        if rule_context == '#stay':
            output.append(f'    "{escaped_name}" -> "{escaped_ctx_name}" [color=dodgerblue3];\n')
        elif rule_context:
            escaped_rule_context = escape(rule_context)
            if rule_context.startswith('#'):
                escaped_bind_ctx_name = jumpctx(escaped_rule_context)
                ref = f'{escaped_ctx_name}!!{escaped_rule_context}'
                output.append(
                    f'    "{escaped_name}" -> "{ref}" [color={color}];\n'
                    f'    "{ref}" [label="{labelize(escaped_rule_context)}"];\n'
                )
                if escaped_bind_ctx_name:
                    kdot[f'{ref}!!{escaped_bind_ctx_name}'] = (
                        f'  "{ref}" -> "{escaped_bind_ctx_name}" [color={color}];\n'
                        f'  "{ref}" [color=red];\n',
                        irule,
                    )
            else:
                kdot[f'{irule}'] = (
                    f'  "{escaped_name}" -> "{escaped_rule_context}" [color={color}];\n',
                    irule,
                )

    def end_ctx(p: XMLParser):
        color = p.ctx_color
        escaped_ctx_name = p.escaped_ctx_name

        fallthrough_ctx = p.ctx_attrs.get('fallthroughContext', '#stay')
        escaped_fallthrough_ctx = escape(fallthrough_ctx)
        push_context_attr(output, escaped_name, escaped_ctx_name,
                          escaped_fallthrough_ctx, 'dashed', color)

        end_ctx = p.ctx_attrs.get('lineEndContext', '#stay')
        escaped_end_ctx = escape(end_ctx)
        push_context_attr(output, escaped_ctx_name, escaped_ctx_name,
                          escaped_end_ctx, 'dotted', 'blue')

        empty_ctx = p.ctx_attrs.get('lineEmptyContext', '#stay')
        escaped_empty_ctx = escape(empty_ctx)
        push_context_attr(output, escaped_ctx_name, escaped_ctx_name,
                          escaped_empty_ctx, 'dotted', 'purple')

        output.append('  }\n')

        push_last_transition(output, escaped_name, escaped_ctx_name,
                             escaped_fallthrough_ctx, color)

        push_last_transition(output, escaped_name, escaped_ctx_name,
                             escaped_end_ctx, color)

        push_last_transition(output, escaped_name, escaped_ctx_name,
                             escaped_empty_ctx, color)

        output.extend(expr for expr, _ in sorted(kdot.values(), key=lambda t: t[1]))


xml_parser = XMLParser(start_ctx, end_ctx, rule_process)
p = expat.ParserCreate()
p.StartElementHandler = xml_parser.start_element
p.EndElementHandler = xml_parser.end_element
if not resolve_entities:
    xml_parser.resolve_entities = False
    p.EntityDeclHandler = xml_parser.entity_decl
    p.EndDoctypeDeclHandler = xml_parser.end_doctype

# # remove BOM
# if content.startswith('\xef\xbb\xbf'):
#     content = content[3:]
p.Parse(Path(args.syntax).read_text())

output.append('}\n')

print(''.join(output))
