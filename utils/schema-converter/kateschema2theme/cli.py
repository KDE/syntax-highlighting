# -*- coding: utf-8 -*-
#
# SPDX-FileCopyrightText: 2020 Alex Turbov <i.zaufi@gmail.com>
# SPDX-FileContributor: Juraj Oravec <jurajoravec@mailo.com>
# SPDX-License-Identifier: MIT
#

'''
CLI utility to convert old `kate`s schema/hlcolors files to the
new JSON theme format.
'''

from __future__ import annotations

import configparser
import enum
import functools
import itertools
import json
import pathlib
import re
import textwrap

from xml.etree import ElementTree
from typing import \
    Dict \
  , Final \
  , Iterable \
  , Generator \
  , List \
  , Literal \
  , Pattern \
  , Set \
  , TextIO \
  , Tuple \
  , TypedDict \
  , TypeVar

import click
import columnize                                            # type: ignore

# BEGIN Type declarations
T = TypeVar('T')                                            # pylint: disable=invalid-name
PropName = Literal[
    'background-color'
  , 'bold'
  , 'italic'
  , 'selected-text-color'
  , 'seleted-background-color'
  , 'strike-through'
  , 'text-color'
  , 'text-color'
  , 'underline'
  ]
StylePropsDict = TypedDict(
    'StylePropsDict'
  , {
        'background-color': str
      , 'bold': bool
      , 'italic': bool
      , 'selected-text-color': str
      , 'seleted-background-color': str
      , 'strike-through': bool
      , 'text-color': str
      , 'underline': bool
    }
  , total=False
  )
CustomStyleDict = Dict[str, StylePropsDict]
CustomStylesDict = Dict[str, CustomStyleDict]
EditorColorsDict = Dict[str, str]
TextStylesDict = Dict[str, StylePropsDict]
MetadataDict = TypedDict(
    'MetadataDict'
  , {
        'name': str
      , 'revision': int
    }
  )
ThemeDict = TypedDict(
    'ThemeDict'
  , {
        'custom-styles': CustomStylesDict
      , 'editor-colors': EditorColorsDict
      , 'metadata': MetadataDict
      , 'text-styles': TextStylesDict
    }
  , total=False
  )
SyntaxesDict = Dict[str, Set[str]]
# END Type declarations


class QtColorItemOffset(enum.IntEnum):
    '''
    Enumeration class with offsets in the CSV record
    of the old style definition.
    '''
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


_EXPECTED_OLD_COLOR_LEN: Final[int] = 8
_OLD_COLOR_LEADING_STRIP_SIZE: Final[int] = 2
_HIGHLIGHTING_PFX: Final[str] = 'Highlighting '
_COLUMIZED_LIST_INDENT_PFX: Final[str] = '   '
_EDITOR_COLORS: Final[Dict[str, str]] = {
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
_TEXT_STYLES: Final[Dict[str, str]] = {
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
_OFFSET2NAME: Final[Dict[QtColorItemOffset, PropName]] = {
    QtColorItemOffset.TEXT: 'text-color'
  , QtColorItemOffset.SELECTED_TEXT: 'selected-text-color'
  , QtColorItemOffset.BOLD: 'bold'
  , QtColorItemOffset.ITALIC: 'italic'
  , QtColorItemOffset.STRIKE_THROUGH: 'strike-through'
  , QtColorItemOffset.UNDERLINE: 'underline'
  , QtColorItemOffset.BACKGROUND: 'background-color'
  , QtColorItemOffset.SELECTED_BACKGROUND: 'seleted-background-color'
  }
_META_SECTIONS: Final[List[str]] = ['KateSchema', 'KateHLColors']
_SECTION_MATCH: Final[Pattern] = re.compile(r'\[(?P<header>[^]]+?)( - Schema .*)?\]')


@click.command()
@click.help_option(
    '--help'
  , '-h'
  )
@click.version_option()
@click.option(
    '--skip-included'
  , '-d'
  , default=True
  , is_flag=True
  , help='Do not write custom colors included from another syntax files.'
  )
@click.option(
    '-s'
  , '--syntax-dirs'
  , multiple=True
  , metavar='DIRECTORY...'
  , type=click.Path(exists=True, file_okay=False, dir_okay=True)
  , help='Specify the directory to search for syntax files. '
         'If given, extra validation going to happen. Multiple '
         'options allowed.'
  )
@click.argument(
    'input-file'
  , type=click.File('r')
  , default='-'
  )
def kateschema2theme(skip_included: bool, syntax_dirs: List[click.Path], input_file: TextIO) -> int:
    ''' Kate colors/schema to theme converter. '''
    config = configparser.ConfigParser(
        delimiters=['=']
      , interpolation=None
      )
    setattr(config, 'optionxform', str)
    setattr(config, 'SECTCRE', _SECTION_MATCH)

    try:
        config.read_file(input_file)
    except configparser.DuplicateOptionError as ex:
        eerror(f'{ex!s}')
        return 1

    result: ThemeDict = {}
    sections: List[str] = config.sections()

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

    custom_styles: CustomStylesDict = functools.reduce(
        collect_custom_colors
      , hl_colors(config, skip_included)
      , {}
      )

    if bool(custom_styles):
        known_syntaxes: SyntaxesDict = get_syntaxes_available(syntax_dirs) \
            if bool(syntax_dirs) else {}
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
    return 0


def convert_editor_color(state: Dict[str, str], color_line: Tuple[str, str]) -> Dict[str, str]:
    '''Convert standard editor color names from old to new using the mapping table.'''
    name, color_settings = color_line
    assert name in _EDITOR_COLORS
    state[_EDITOR_COLORS[name]] = decode_rgb_set(color_settings)
    return state


def decode_rgb_set(color_settings: str) -> str:
    '''Transform the RGB record given as CSV string to web-hex format.'''
    return rgb2hex(*map(int, color_settings.split(',')))


def rgb2hex(red: int, green: int, blue: int) -> str:
    '''Convert R,G,B integers to web-hex string'''
    return f'#{red:02x}{green:02x}{blue:02x}'


def collect_standard_colors(state, item):
    '''Convert standard text styles from old to new names using the mapping table.'''
    name, value = item
    state[_TEXT_STYLES[name]] = parse_qcolor_value(value)
    return state


def collect_custom_colors(state: CustomStylesDict, item: Tuple[str, str, str]) -> CustomStylesDict:
    '''A functor to convert one old style setting to the new format
        and update the given `state` (a dict).
    '''
    syntax, syntax_item, value = item

    props = parse_qcolor_value(value)
    if bool(props):
        syntax_node: CustomStyleDict = state.get(syntax, {})
        syntax_node[syntax_item] = props
        state[syntax] = syntax_node

    return state


def hl_colors(config: configparser.ConfigParser, skip_included: bool) \
  -> Generator[Tuple[str, str, str], None, None]:
    '''A generator function to iterate over custom styles in the old format.'''
    for section in config.sections():
        if not section.startswith(_HIGHLIGHTING_PFX):
            continue

        for name, value in config.items(section):
            syntax, *parts = name.split(':')
            if not bool(parts):
                ewarn(f'Unexpected color name: `{name}` in section `{section}`')

            elif not skip_included or section[len(_HIGHLIGHTING_PFX):] == syntax:
                yield syntax, ':'.join(parts), value


def parse_qcolor_value(value: str) -> StylePropsDict:
    '''Convert old color settings (QColor stored as a CSV config item)
        into a dict of new styles.
    '''
    components = value.split(',')
    if len(components) == QtColorItemOffset.CUSTOM_COLOR_EXPECTED_SIZE:
        components.pop()
    assert len(components) == QtColorItemOffset.STANDARD_COLOR_EXPECTED_SIZE
    return transform_qcolor_to_dict(components)


def transform_qcolor_to_dict(components: List[str]) -> StylePropsDict:
    '''Convert old color settings given as a list of items
        into a dict of new styles.
    '''
    init: StylePropsDict = {}
    return functools.reduce(convert_color_property, enumerate(components), init)


def convert_color_property(state: StylePropsDict, prop: Tuple[int, str]) -> StylePropsDict:
    '''A reducer functor to convert one item of the former color record (CSV)
        into a new property name and a value.
    '''
    offset = QtColorItemOffset(prop[0])
    value = prop[1]
    assert offset < QtColorItemOffset.CUSTOM_COLOR_EXPECTED_SIZE

    if bool(value) and offset in _OFFSET2NAME:
        custom_prop_name = _OFFSET2NAME[offset]
        if custom_prop_name.endswith('-color'):
            if len(value) == _EXPECTED_OLD_COLOR_LEN:
                state[custom_prop_name] = '#' + value[_OLD_COLOR_LEADING_STRIP_SIZE:]
        else:
            state[custom_prop_name] = bool(value == '1')

    return state


def first_true(pred, iterable: Iterable[T], default=None) -> T:
    '''A helper function to return first item for which predicate is true.'''
    return next(filter(pred, iterable), default)


def get_syntaxes_available(dirs: List[click.Path]) -> SyntaxesDict:
    '''Collect syntaxs available in the given path.

        Returns a dict of syntax names to a list of syntax items in it.
    '''
    return functools.reduce(
        load_syntax_data
      , filter(
            lambda p: p.suffix == '.xml'
          , itertools.chain(
                *map(
                    lambda p: pathlib.Path(str(p)).iterdir()
                  , dirs
                  )
              )
          )
      , {}
      )


def load_syntax_data(state: SyntaxesDict, syntax_file: pathlib.Path) -> SyntaxesDict:
    '''A reducer functor to obtain syntax items.'''
    tree = ElementTree.parse(syntax_file)
    root = tree.getroot()

    syntax_name = root.get('name')
    assert syntax_name is not None

    if syntax_name in state:
        ewarn(
            f'Use `{syntax_name}` found '
            f'in `{click.format_filename(str(syntax_file))}`'
          )

    state[syntax_name] = functools.reduce(
        collect_syntax_item_data
      , root.iterfind('highlighting/itemDatas/itemData')
      , set()
      )
    return state


def verify_converted_styles(custom_styles: CustomStylesDict, known_syntaxes: SyntaxesDict) \
  -> CustomStylesDict:
    '''Validate the given `custom_styles` according to actual syntax items
        described in the known syntax files.

        Returns a dict of syntaxes without unused syntax items.
    '''
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


def remove_unused_syntax_item(state: CustomStyleDict, item: str) -> CustomStyleDict:
    '''Remove the given `item` from the `state`.'''
    assert item in state
    del state[item]
    return state


def format_columns(iterable: Iterable[str]) -> str:
    '''A helper functor to output the list in columns.'''
    term_width = click.get_terminal_size()[0] - len(_COLUMIZED_LIST_INDENT_PFX)
    return textwrap.indent(
        columnize.columnize(iterable, displaywidth=term_width, colsep=' │ ')
      , prefix=_COLUMIZED_LIST_INDENT_PFX
      )


def collect_syntax_item_data(items: Set[str], node: ElementTree.Element) -> Set[str]:
    '''A reducer functor to append a syntax item name to the given set.'''
    name = node.get('name')
    assert name is not None
    items.add(name)

    return items


def eerror(msg: str):
    '''A helper function to display an error message.'''
    click.echo(' ' + click.style('*', fg='red', bold=True) + f' {msg}', err=True)


def ewarn(msg: str):
    '''A helper function to display a warning message.'''
    click.echo(' ' + click.style('*', fg='yellow') + f' {msg}', err=True)
