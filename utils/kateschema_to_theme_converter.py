#!/usr/bin/env python3
"""
LICENSE:
SPDX-FileCopyrightText: 2020 Juraj Oravec <jurajoravec@mailo.com>
SPDX-License-Identifier: MIT

Configuration:
- IncludeCustoms: Try to export all available settings
- prefferStandAloneData: prefferdata set specifically for current datatype

Usage:
script.py ThemeName themefile.kateschema
"""

from configparser import ConfigParser
import json
import sys


settings = {
    "prefferStandAloneData": True,
    "IncludeCustoms": True
}


jsonConfig = {
    "_comments": "Created by theme_converter script"
}
editorColors = {
    "Color Background": "BackgroundColor",
    "Color Code Folding": "CodeFolding",
    "Color Current Line Number": "CurrentLineNumber",
    "Color Highlighted Bracket": "BracketMatching",
    "Color Highlighted Line": "CurrentLine",
    "Color Icon Bar": "IconBorder",
    "Color Indentation Line": "IndentationLine",
    "Color Line Number": "LineNumbers",
    "Color MarkType 1": "MarkBookmark",
    "Color MarkType 2": "MarkBreakpointActive",
    "Color MarkType 3": "MarkBreakpointReached",
    "Color MarkType 4": "MarkBreakpointDisabled",
    "Color MarkType 5": "MarkExecution",
    "Color MarkType 6": "MarkWarning",
    "Color MarkType 7": "MarkError",
    "Color Modified Lines": "ModifiedLines",
    "Color Replace Highlight": "ReplaceHighlight",
    "Color Saved Lines": "SavedLines",
    "Color Search Highlight": "SearchHighlight",
    "Color Selection": "TextSelection",
    "Color Separator": "Separator",
    "Color Spelling Mistake Line": "SpellChecking",
    "Color Tab Marker": "TabMarker",
    "Color Template Background": "TemplateBackground",
    "Color Template Editable Placeholder": "TemplatePlaceholder",
    "Color Template Focused Editable Placeholder": "TemplateFocusedPlaceholder",
    "Color Template Not Editable Placeholder": "TemplateReadOnlyPlaceholder",
    "Color Word Wrap Marker": "WordWrapMarker"
}
textStyles = {
    "Alert": "Alert",
    "Annotation": "Annotation",
    "Attribute": "Attribute",
    "Base-N Integer": "BaseN",
    "Built-in": "BuiltIn",
    "Character": "Char",
    "Comment": "Comment",
    "Comment Variable": "CommentVar",
    "Constant": "Constant",
    "Control Flow": "ControlFlow",
    "Data Type": "DataType",
    "Decimal/Value": "DecVal",
    "Documentation": "Documentation",
    "Error": "Error",
    "Extension": "Extension",
    "Floating Point": "Float",
    "Function": "Function",
    "Import": "Import",
    "Information": "Information",
    "Keyword": "Keyword",
    "Normal": "Normal",
    "Operator": "Operator",
    "Others": "Others",
    "Preprocessor": "Preprocessor",
    "Region Marker": "RegionMarker",
    "Special Character": "SpecialChar",
    "Special String": "SpecialString",
    "String": "String",
    "Variable": "Variable",
    "Verbatim String": "VerbatimString",
    "Warning": "Warning"
}
indexToStyle = {
    "0": "Normal",
    "1": "Keyword",
    "2": "Function",
    "3": "Variable",
    "4": "ControlFlow",
    "5": "Operator",
    "6": "BuiltIn",
    "7": "Extension",
    "8": "Preprocessor",
    "9": "Attribute",
    "10": "Char",
    "11": "SpecialChar",
    "12": "String",
    "13": "VerbatimString",
    "14": "SpecialString",
    "15": "Import",
    "16": "DataType",
    "17": "DecVal",
    "18": "BaseN",
    "19": "Float",
    "20": "Constant",
    "21": "Comment",
    "22": "Documentation",
    "23": "Annotation",
    "24": "CommentVar",
    "25": "RegionMarker",
    "26": "Information",
    "27": "Warning",
    "28": "Alert",
    "29": "Others",
    "30": "Error"
}

NormalizedSections = dict[str, str]
Style = dict[str, str | bool]
CustomStyles = dict[str, Style]


def normalizeSections(sections: list[str]) -> NormalizedSections:
    return {value.partition(" - ")[0]: value for value in sections}


def reEcodeColors(text: str) -> str:
    return "#" + text[2:]


def reEncodeBool(text: str) -> bool:
    return text == "1"


def rgb_to_hex(rgb: tuple) -> str:
    return '#%02x%02x%02x' % rgb


def decodeTextStyle(text: str) -> Style:
    style = {}

    field = text.split(",")

    if len(field) == 11:
        styleIndex = field.pop(0)
        style = jsonConfig["text-styles"].get(indexToStyle[styleIndex], dict()).copy()

    if len(field) != 10 or not any(field[0:8]):
        return dict()

    if len(field[0]) == 8:
        style["text-color"] = reEcodeColors(field[0])
    if len(field[1]) == 8:
        style["selected-text-color"] = reEcodeColors(field[1])
    if len(field[2]) == 1:
        style["bold"] = reEncodeBool(field[2])
    if len(field[3]) == 1:
        style["italic"] = reEncodeBool(field[3])
    if len(field[4]) == 1:
        style["strike-through"] = reEncodeBool(field[4])
    if len(field[5]) == 1:
        style["underline"] = reEncodeBool(field[5])
    if len(field[6]) == 8:
        style["background-color"] = reEcodeColors(field[6])
    if len(field[7]) == 8:
        style["selected-text-color"] = reEcodeColors(field[7])
    # 8: font family > ignored
    # 9: --- > ignored

    return style


def decodeColorSettings(text: str) -> str | None:
    fieldds = tuple(map(int, text.split(",")))

    if len(fieldds) != 3:
        return

    return rgb_to_hex(fieldds)


def extractEditorColors(section: dict[str, str]) -> dict[str, str | None]:
    return {editorColors[key]: decodeColorSettings(value)
            for key, value in section.items()}



def extractTextStyles(section: dict[str, str]) -> dict[str, Style]:
    return {textStyles[key]: decodeTextStyle(value)
            for key, value in section.items()}


def extractCustomStyle(custom_styles: CustomStyles, style: Style, realKey: str):
    for key, value in style.items():
        style = decodeTextStyle(value)

        # some items have ':' in their name, therefore it is necessary to limit the split
        keys = key.split(":", 1)
        # invalid or None language
        if len(keys) == 1:
            continue

        primaryKey, SecondaryKey = keys

        if style:
            custom_style = custom_styles.setdefault(primaryKey, dict())
            custom_style.setdefault(SecondaryKey, style)

            if settings["prefferStandAloneData"] and realKey == primaryKey:
                custom_style[SecondaryKey] = style


def extractCustomStyles(config: ConfigParser, normalizedSections: NormalizedSections) -> CustomStyles:
    custom_styles: CustomStyles = {}

    for key, value in normalizedSections.items():
        if not key.startswith("Highlighting"):
            continue

        realKey = key[len("Highlighting "):]

        extractCustomStyle(custom_styles, config[value], realKey)

    return custom_styles


def main(inputFile: str):
    config = ConfigParser(delimiters="=")
    config.optionxform = str
    config.read(inputFile)

    normalizedSections = normalizeSections(config.sections())

    if "Editor Colors" in normalizedSections:
        jsonConfig["editor-colors"] = extractEditorColors(config[normalizedSections["Editor Colors"]])
    if "Default Item Styles" in normalizedSections:
        jsonConfig["text-styles"] = extractTextStyles(config[normalizedSections["Default Item Styles"]])

    if settings["IncludeCustoms"]:
        jsonConfig["custom-styles"] = extractCustomStyles(config, normalizedSections)

    print(json.dumps(jsonConfig, indent=4, sort_keys=True))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: " + sys.argv[0] + " ThemeName Filepath.kateschema")
        exit()

    jsonConfig["metadata"] = {
        "name": str(sys.argv[1]),
        "revision": 1
    }

    main(sys.argv[2])
