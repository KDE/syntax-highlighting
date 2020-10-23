# -*- coding: utf-8 -*-
#
# SPDX-FileCopyrightText: 2020 Alex Turbov <i.zaufi@gmail.com>
# SPDX-License-Identifier: MIT
#

import configparser
import enum
import functools
import json
import pathlib
import re
import textwrap
import sys

from xml.etree import ElementTree
from typing import List

import click
import columnize


class QColorItemOffset(enum.IntEnum):
    _UNKNOWN = 0
    TEXT = enum.auto()
    SELECTED_TEXT = enum.auto()
    BOLD = enum.auto()
    ITALIC = enum.auto()
    STRIKE_THROUGH = enum.auto()
    UNDERLINE = enum.auto()
    BACKGROUND = enum.auto()
    SELECTED_BACKGROUND = enum.auto()
    _IGNORED_FONT_FAMILY = enum.auto()
    _TRAILING_DASHES = enum.auto()
    # Special item to validate the components count
    CUSTOM_COLOR_EXPECTED_SIZE = enum.auto()
    STANDARD_COLOR_EXPECTED_SIZE = 10


_EXPECTED_OLD_COLOR_LEN = 8
_OLD_COLOR_LEADING_STRIP_SIZE = 2
_HIGHLIGHTING_PFX = 'Highlighting '
_COLUMIZED_LIST_INDENT_PFX = '   '
_EDITOR_COLORS = {
    "Color Background": "BackgroundColor"
  , "Color Code Folding": "CodeFolding"
  , "Color Current Line Number": "CurrentLineNumber"
  , "Color Highlighted Bracket": "BracketMatching"
  , "Color Highlighted Line": "CurrentLine"
  , "Color Icon Bar": "IconBorder"
  , "Color Indentation Line": "IndentationLine"
  , "Color Line Number": "LineNumbers"
  , "Color MarkType 1": "MarkBookmark"
  , "Color MarkType 2": "MarkBreakpointActive"
  , "Color MarkType 3": "MarkBreakpointReached"
  , "Color MarkType 4": "MarkBreakpointDisabled"
  , "Color MarkType 5": "MarkExecution"
  , "Color MarkType 6": "MarkWarning"
  , "Color MarkType 7": "MarkError"
  , "Color Modified Lines": "ModifiedLines"
  , "Color Replace Highlight": "ReplaceHighlight"
  , "Color Saved Lines": "SavedLines"
  , "Color Search Highlight": "SearchHighlight"
  , "Color Selection": "TextSelection"
  , "Color Separator": "Separator"
  , "Color Spelling Mistake Line": "SpellChecking"
  , "Color Tab Marker": "TabMarker"
  , "Color Template Background": "TemplateBackground"
  , "Color Template Editable Placeholder": "TemplatePlaceholder"
  , "Color Template Focused Editable Placeholder": "TemplateFocusedPlaceholder"
  , "Color Template Not Editable Placeholder": "TemplateReadOnlyPlaceholder"
  , "Color Word Wrap Marker": "WordWrapMarker"
  }
_TEXT_STYLES = {
    "Alert": "Alert"
  , "Annotation": "Annotation"
  , "Attribute": "Attribute"
  , "Base-N Integer": "BaseN"
  , "Built-in": "BuiltIn"
  , "Character": "Char"
  , "Comment": "Comment"
  , "Comment Variable": "CommentVar"
  , "Constant": "Constant"
  , "Control Flow": "ControlFlow"
  , "Data Type": "DataType"
  , "Decimal/Value": "DecVal"
  , "Documentation": "Documentation"
  , "Error": "Error"
  , "Extension": "Extension"
  , "Floating Point": "Float"
  , "Function": "Function"
  , "Import": "Import"
  , "Information": "Information"
  , "Keyword": "Keyword"
  , "Normal": "Normal"
  , "Operator": "Operator"
  , "Others": "Others"
  , "Preprocessor": "Preprocessor"
  , "Region Marker": "RegionMarker"
  , "Special Character": "SpecialChar"
  , "Special String": "SpecialString"
  , "String": "String"
  , "Variable": "Variable"
  , "Verbatim String": "VerbatimString"
  , "Warning": "Warning"
  }
_OFFSET2NAME = {
    QColorItemOffset.TEXT: 'text-color'
  , QColorItemOffset.SELECTED_TEXT: 'selected-text-color'
  , QColorItemOffset.BOLD: 'bold'
  , QColorItemOffset.ITALIC: 'italic'
  , QColorItemOffset.STRIKE_THROUGH: 'strike-through'
  , QColorItemOffset.UNDERLINE: 'underline'
  , QColorItemOffset.BACKGROUND: 'background-color'
  , QColorItemOffset.SELECTED_BACKGROUND: 'seleted-background-color'
  }
_META_SECTIONS = ['KateSchema', 'KateHLColors']
_SECTION_MATCH = re.compile(r'\[(?P<header>[^]]+?)( - Schema .*)?\]')


@click.command()
@click.help_option(
    '--help'
  , '-h'
  )
@click.version_option(
    prog_name='Kate colors/schema to theme converter'
  )
@click.option(
    '--skip-included'
  , '-d'
  , default=True
  , is_flag=True
  , help='Do not write custom colors included from another syntax files.'
  )
@click.option(
    '-s'
  , '--syntax-dir'
  , help='Specify the directory to search for syntax files. '
         'If given, extra validation going to happen.'
  )
@click.argument(
    'input-file'
  , type=click.File('r')
  , default='-'
  )
def kateschema2theme(skip_included, syntax_dir, input_file):
    config = configparser.ConfigParser(delimiters=['='])
    config.optionxform = str
    config.SECTCRE = _SECTION_MATCH

    try:
        config.read_file(input_file)
    except configparser.DuplicateOptionError as ex:
        eerror(f'{ex!s}')
        return 1

    result = {}
    sections = config.sections()

    if 'Editor Colors' in sections:
        result['editor-colors'] = functools.reduce(
            convert_editor_color
          , config.items('Editor Colors')
          , {}
          )

    if 'Default Item Styles' in sections:
        result['text-styles'] = functools.reduce(
            collect_standard_colors
          , config.items('Default Item Styles')
          , {}
          )

    custom_styles = functools.reduce(
        collect_custom_colors
      , hl_colors(config, skip_included)
      , {}
      )

    if bool(custom_styles):
        known_syntaxes = get_syntaxes_available(syntax_dir) if bool(syntax_dir) else {}
        if bool(known_syntaxes):
            custom_styles = verify_converted_styles(custom_styles, known_syntaxes)

        result['custom-styles'] = custom_styles

    meta_section_name = first_true(lambda name: name in sections, _META_SECTIONS)
    if meta_section_name is not None:
        result['metadata'] = {
            'name': config[meta_section_name]['schema']
          , 'revision': 1
          }

    print(json.dumps(result, sort_keys=True, indent=4))


def convert_editor_color(state, color_line):
    name, color_settings = color_line
    assert name in _EDITOR_COLORS
    state[_EDITOR_COLORS[name]] = decode_rgb_set(color_settings)
    return state


def decode_rgb_set(color_settings):
    return rgb2hex(*map(int, color_settings.split(',')))


def rgb2hex(r, g, b) -> str:
    return f'#{r:02x}{g:02x}{b:02x}'


def collect_standard_colors(state, item):
    name, value = item
    state[_TEXT_STYLES[name]] = parse_qcolor_value(value)
    return state


def collect_custom_colors(state, item):
    syntax, color_name, value = item

    props = parse_qcolor_value(value)
    if bool(props):
        syntax_node = state.get(syntax, {})
        syntax_node[color_name] = props
        state[syntax] = syntax_node

    return state


def hl_colors(config, skip_included):
    for section in config.sections():
        if not section.startswith(_HIGHLIGHTING_PFX):
            continue

        for name, value in config.items(section):
            syntax, *parts = name.split(':')
            if not bool(parts):
                ewarn(f'Unexpected color name: `{name}` in section `{section}`')
            elif not skip_included or section[len(_HIGHLIGHTING_PFX):] == syntax:
                yield syntax, ':'.join(parts), value


def parse_qcolor_value(value: str):
    components = value.split(',')
    if len(components) == QColorItemOffset.CUSTOM_COLOR_EXPECTED_SIZE:
        components.pop()
    assert len(components) == QColorItemOffset.STANDARD_COLOR_EXPECTED_SIZE
    return transform_qcolor_to_dict(components)


def transform_qcolor_to_dict(components: List[str]):
    return functools.reduce(convert_color_property, enumerate(components), {})


def convert_color_property(state, prop):
    offset, value = prop
    assert offset < QColorItemOffset.CUSTOM_COLOR_EXPECTED_SIZE

    if bool(value) and offset in _OFFSET2NAME:
        custom_prop_name = _OFFSET2NAME[offset]
        if custom_prop_name.endswith('-color'):
            if len(value) == _EXPECTED_OLD_COLOR_LEN:
                state[custom_prop_name] = '#' + value[_OLD_COLOR_LEADING_STRIP_SIZE:]
        else:
            state[custom_prop_name] = True if value == '1' else False

    return state


def first_true(pred, iterable, default=None):
    return next(filter(pred, _META_SECTIONS), default)


def get_syntaxes_available(base: str):
    return functools.reduce(
        load_syntax_data
      , filter(
            lambda p: p.suffix == '.xml'
          , pathlib.Path(base).iterdir()
          )
      , {}
      )


def load_syntax_data(state, syntax_file: pathlib.Path):
    tree = ElementTree.parse(syntax_file)
    root = tree.getroot()

    syntax_name = root.get('name')
    assert syntax_name is not None

    if not syntax_name in state:
        state[syntax_name] = functools.reduce(
            collect_syntax_item_data
          , root.iterfind('highlighting/itemDatas/itemData')
          , set()
          )
    else:
        ewarn(
            f'Ignore redefinition of `{syntax_name}` found '
            f'in `{click.format_filename(syntax_file, shorten=True)}`'
          )

    return state


def verify_converted_styles(custom_styles, known_syntaxes):
    for syntax, styles in custom_styles.items():
        if syntax not in known_syntaxes:
            ewarn(f'The `{syntax}` is not known. Ignoring validation.')
            continue

        found_custom_items = set(styles.keys())

        if unused_items := [*found_custom_items.difference(known_syntaxes[syntax])]:
            ewarn(
                f'The following styles are not used by `{syntax}` syntax anymore:'
              + '\n'
              + format_columns(unused_items)
              )
            custom_styles[syntax] = functools.reduce(
                remove_unused_syntax_item
              , unused_items
              , styles
              )

        if undefined_items := [*known_syntaxes[syntax].difference(found_custom_items)]:
            ewarn(
                f'The following styles are not defined in the converted `{syntax}` syntax:'
              + '\n'
              + format_columns(undefined_items)
              )

    return custom_styles


def remove_unused_syntax_item(state, item):
    assert item in state
    del state[item]
    return state


def format_columns(iterable):
    term_width = click.get_terminal_size()[0] - len(_COLUMIZED_LIST_INDENT_PFX)
    return textwrap.indent(
        columnize.columnize(iterable, displaywidth=term_width, colsep=' │ ')
      , prefix=_COLUMIZED_LIST_INDENT_PFX
      )


def collect_syntax_item_data(items, node):
    items.add(node.get('name'))
    return items


def eerror(msg):
    click.echo(' ' + click.style('*', fg='red', bold=True) + f' {msg}', err=True)


def ewarn(msg):
    click.echo(' ' + click.style('*', fg='yellow') + f' {msg}', err=True)
