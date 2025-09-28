#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2023 Jonathan Poelen <jonathan.poelen@gmail.com>
# SPDX-License-Identifier: MIT

from argparse import ArgumentParser
from pathlib import Path
from typing import Iterable, Mapping, Iterable
from textwrap import wrap
from xml.parsers import expat
from abc import ABC, abstractmethod

import re


parser = ArgumentParser(
    prog='generate-dot-file.py',
    description=f'''Dot file generator for xml syntax

Example:
    generate-dot-file.py data/syntax/lua.xml | dot -T svg -o image.svg''')

parser.add_argument('-c', '--context-only', action='store_true',
                    help='Generates contexts without rules')

parser.add_argument('-n', '--minimize-node', action='store_true',
                    help='Removes intermediate nodes that appear in the presence of #pop or multiple contexts (only with --context-only)')

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
minimize_node = args.minimize_node
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

class XMLParserBase(ABC):
    depth = Outside
    matched = False
    ictx = 0
    ctx_name = ''
    ctx_attrs: dict[str, str]
    escaped_ctx_name = ''
    ctx_color = ''
    irule = 0

    resolve_entities = True
    reversed_entities: dict[str, str] = {}
    resolved_entity_searcher: re.Pattern

    output: list[str]
    minimize_node: bool

    def __init__(self, output: list[str]):
        self.output = output
        self.ctx_attrs = {}
        self.reversed_entities = {}
        super().__init__()

    @abstractmethod
    def start_ctx(self) -> None:
        ...

    @abstractmethod
    def end_ctx(self) -> None:
        ...

    @abstractmethod
    def rule_process(self, irule: int, tag: str, attrs: dict[str, str]) -> None:
        ...

    def start_element(self, tag: str, attrs: dict[str, str]):
        if self.depth == Context:
            self.depth = Rule
            if self.matched:
                self.irule += 1
                if not self.resolve_entities:
                    string = attrs.get('String')
                    if string:
                        attrs['String'] = self.unresolve_entities(string)
                self.rule_process(self.irule, tag, attrs)
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
                self.start_ctx()

    def end_element(self, name: str):
        if self.depth == Context:
            if self.matched:
                self.end_ctx()
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


color_map = (
    '"/rdgy4/3"',
    '"/set312/1"',
    '"lightgoldenrod2"',
    '"/set312/3"',
    '"/set312/4"',
    '"/set312/5"',
    '"/set312/6"',
    '"/set312/7"',
    '"/rdpu3/2"',
    '"/purd6/3"',
    '"/ylgn4/2"',
    '"/set26/6"',
)

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


_pops_re = re.compile(r'^(?:#pop)+!?')

def split_ctx(ctx: str) -> tuple[str,  # '#pop' sequence
                                 str,  # seprator between pop and ctx
                                 str,  # contexts without pop
                                 Iterable[str]  # next contexts
                                 ]:
    pop = ''

    ctx_without_pop: str = _pops_re.sub('', ctx)
    n = len(ctx) - len(ctx_without_pop)
    if n:
        if n > 5:
            pop = f'#pop({n//4})'
        else:
            pop = '#pop'

    if ctx_without_pop:
        # unique context with order preservation
        ctxs = {ctx: None for ctx in ctx_without_pop.split('!')}
        sep = pop and ctx_without_pop and "!" or ""
        return (pop, sep, ctx_without_pop, ctxs.keys())

    return (pop, '', ctx_without_pop, ())


def stringify_attrs(attr_names: Iterable[str], attrs: Mapping[str, str]) -> str:
    s = ''
    for name in attr_names:
        attr = attrs.get(name)
        if attr:
            part = '\n'.join(wrap(attr, 40))
            s += f'  {name}:{part}'
    return s


def escape(s: str) -> str:
    return s.replace('\\', '\\\\').replace('"', '\\"')


def xml_bool(s: str | None) -> bool:
    return s == '1' or s == 'true'


class XMLContextOnlyParser(XMLParserBase):
    # avoid multi arrow for ctx1 -> ctx2
    krule_contexts: dict[str, int]
    minimize_node: bool

    def __init__(self, output: list[str], minimize_node: bool):
        self.krule_contexts = {}
        self.minimize_node = minimize_node
        super().__init__(output)

    def start_ctx(self) -> None:
        self.krule_contexts.clear()

    def rule_process(self, irule: int, name: str, attrs: dict[str, str]) -> None:
        self.krule_contexts[attrs.get('context') or '#stay'] = irule

    def end_ctx(self) -> None:
        color = self.ctx_color
        ctx_name = self.escaped_ctx_name
        self.output.append(f'  "{ctx_name}" [style=filled,color={color}]\n')

        self.krule_contexts.setdefault(self.ctx_attrs.get('fallthroughContext') or '#stay', -1)
        self.krule_contexts.setdefault(self.ctx_attrs.get('lineEndContext') or '#stay', -2)
        self.krule_contexts.setdefault(self.ctx_attrs.get('lineEmptyContext') or '#stay', -3)

        self.krule_contexts.pop('#stay')

        dejavu: set[str] = set()

        def push(s: str):
            if s not in dejavu:
                dejavu.add(s)
                self.output.append(s)

        for rule_context, i in sorted(self.krule_contexts.items(), key=lambda t: t[1]):
            escaped_rule_context = escape(rule_context)
            pop, sep, ctx_without_pop, next_contexts = split_ctx(escaped_rule_context)

            if self.minimize_node:
                for ctx in next_contexts:
                    push(f'  "{ctx_name}" -> "{ctx}" [color={color}];\n')
                continue

            # rule
            if i >= 0:
                style = ''
            # fallthroughContext
            elif i == -1:
                style = ',style=dashed'
            # lineEndContext
            elif i == -2:
                style = ',style=dotted'
            # lineEmptyContext
            else:  # if i == -3:
                style = ',style=bold'

            # create intermediate node
            if pop or len(next_contexts) > 1:
                node = f'{ctx_name}!!{escaped_rule_context}'
                push(
                    f'  "{ctx_name}" -> "{node}" [color={color}{style}];\n'
                    f'  "{node}" [label="{pop}{sep}{ctx_without_pop}"];\n'
                )
                if node not in dejavu:
                    dejavu.add(node)
                    for ctx in next_contexts:
                        self.output.append(f'  "{node}" -> "{ctx}" [color={color}];\n')
            # direct link
            else:
                push(f'  "{ctx_name}" -> "{escaped_rule_context}" [color={color}{style}];\n')


class XMLParser(XMLParserBase):
    # Cattribute is the context attribute if no attribute is specified
    # note: char1 is tranformed into String
    FIRST_LINE_ATTRIBUTES = ('attribute', 'Cattribute', 'String', 'char')
    SECOND_LINE_ATTRIBUTES = ('beginRegion', 'endRegion', 'lookAhead', 'firstNonSpace', 'column', 'additionalDeliminator', 'weakDeliminator', 'insensitive')

    kdot: dict[str, bool]
    escaped_name = ''

    def __init__(self, output: list[str]):
        self.kdot = {}
        super().__init__(output)

    def start_ctx(self) -> None:
        self.escaped_name = self.escaped_ctx_name

        self.kdot.clear()
        self.output.append(
            f'  subgraph cluster{self.ictx} {{\n'
            f'    "{self.escaped_name}" [shape=box,style=filled,color={self.ctx_color}];\n'
        )

    def rule_process(self, irule: int, name: str, attrs: dict[str, str]) -> None:
        color = self.ctx_color
        escaped_ctx_name = self.escaped_ctx_name

        escaped_name = f'{escaped_ctx_name}!!{irule}!!{escape(name)}'

        # context or rule linked to the next rule
        self.output.append(
            f'    "{self.escaped_name}" -> "{escaped_name}"'
            f' [style=dashed,color={color}];\n'
        )

        self.escaped_name = escaped_name

        rule_context = attrs.get('context', '#stay')

        if name == 'IncludeRules':
            label = f'  {rule_context}'
        else:
            if 'attribute' not in attrs:
                attrs['Cattribute'] = self.ctx_attrs['attribute']
            if 'char1' in attrs:
                attrs['String'] = attrs.pop('char') + attrs.pop('char1')
            label = stringify_attrs(self.FIRST_LINE_ATTRIBUTES, attrs)
            label2 = stringify_attrs(self.SECOND_LINE_ATTRIBUTES, attrs)
            if label2:
                label = f'{label}\n{label2}'
        self.output.append(f'    "{escaped_name}" [label="{name}{escape(label)}"];\n')

        if xml_bool(attrs.get('lookAhead')):
            self.output.append(f'    "{escaped_name}" [style=dashed];\n')

        if rule_context and rule_context != '#stay':
            self.push_link(escaped_name, rule_context, color, style='')
        else:
            self.output.append(
                f'    "{escaped_name}" -> "{escaped_ctx_name}" [color=dodgerblue3];\n'
            )

    def push_link(self, escaped_name: str, ctx: str, color: str, style: str) -> None:
        if style:
            style = f',style={style}'

        escaped_ctx = escape(ctx)
        pop, sep, ctx_without_pop, next_contexts = split_ctx(escaped_ctx)

        if pop or len(next_contexts) > 1:
            node = f'{self.escaped_ctx_name}!!{escaped_ctx}'
            self.kdot[f'    "{escaped_name}" -> "{node}" [color={color}{style}];\n'] = True
            self.kdot[f'    "{node}" [label="{pop}{sep}{ctx_without_pop}",color=red];\n'] = True
            for ctx in next_contexts:
                self.kdot[f'  "{node}" -> "{ctx}" [color={color}];\n'] = False
        else:
            self.kdot[f'  "{escaped_name}" -> "{escaped_ctx}" [color={color}{style}];\n'] = False

    def end_ctx(self) -> None:
        for escaped_ctx, attr, style, color in (
            (self.escaped_name, 'fallthroughContext', 'dashed', self.ctx_color),
            (self.escaped_ctx_name, 'lineEndContext', 'dotted', 'blue'),
            (self.escaped_ctx_name, 'lineEmptyContext', 'bold', 'purple'),
        ):
            ctx = self.ctx_attrs.get(attr)

            if ctx and ctx != '#stay':
                self.push_link(escaped_ctx, ctx, color, style)
            else:
                self.output.append(
                    f'    "{escaped_ctx}" -> "{self.escaped_ctx_name}"'
                    f' [style={style},color={color}];\n'
                )

        self.output.extend(s for s, in_cluster in self.kdot.items() if in_cluster)
        self.output.append('  }\n')
        self.output.extend(s for s, in_cluster in self.kdot.items() if not in_cluster)


output = [
    'digraph G {\n',
    '  compound=true;ratio=auto\n'
]

if context_only:
    xml_parser = XMLContextOnlyParser(output, minimize_node)
else:
    xml_parser = XMLParser(output)

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
