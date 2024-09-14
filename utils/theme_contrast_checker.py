#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2023 Jonathan Poelen <jonathan.poelen@gmail.com>
# SPDX-License-Identifier: MIT

import argparse
import json
import sys

def parse_scores(s: str) -> list[int]:
    return [int(x) for x in s.split(',')]


parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter,
                                 description='''Contrast checker for themes

Allows you to view all the colors and backgrounds applied to a theme and rate the contrast based on the APCA (Accessible Perceptual Contrast Algorithm) used in WCAG 3. A very low score is a sign of poor contrast, and can make reading difficult or impossible.

However, color perception depends on the individual, hardware or software configurations (night/blue light filter), lighting or simply surrounding colors. For example, low contrast may remain legible in the editor when not surrounded by bright color.

There are 3 options for modifying the contract result:

  -a / --add-luminance and -p / --add-percent-luminance to directly modify the output value. For example, -p -14 -a 15 increases the constrast a little when it's low and very little when it's high.

  -C / --color-space Okl selects a color space with different properties, mainly on the color red.
''')

parser.add_argument('-f', '--bg', metavar='BACKGROUND', action='append',
                    help='show only the specified background color styles')
parser.add_argument('-l', '--language', metavar='LANGUAGE', action='append',
                    help='show only the specified language')

parser.add_argument('-c', '--no-custom-styles', action='store_true',
                    help='do not display custom languages')
parser.add_argument('-s', '--no-standard-styles', action='store_true',
                    help='do not display standard colors')
parser.add_argument('-b', '--no-borders', action='store_true',
                    help='do not display border colors')
parser.add_argument('-H', '--no-legend', action='store_true',
                    help='do not display legend')

parser.add_argument('-M', '--min-luminance', metavar='LUMINANCE', type=float, default=0,
                    help='only displays colors with a higher luminance')
parser.add_argument('-L', '--max-luminance', metavar='LUMINANCE', type=float, default=110.0,
                    help='only displays colors with a lower luminance')

parser.add_argument('-a', '--add-luminance', metavar='LUMINANCE', type=float, default=0,
                    help='add fixed value for luminance')
parser.add_argument('-p', '--add-percent-luminance', metavar='LUMINANCE', type=float, default=0,
                    help='add percent luminance. Apply before --add-luminance')
parser.add_argument('-S', '--scores', metavar='SCORES', type=parse_scores,
                    help='modify ratings values. Expects a comma-separated list of numbers. The order of the list is the same as in the legend.')

# sRGB is W3 in APCA
parser.add_argument('-C', '--color-space', default='sRGB',
                    choices=['sRGB', 'DisplayP3', 'AdobeRGB', 'Rec2020', 'Okl'],
                    help='select a color space ; Okl is a color space that increases the contrast of red with black or blue background and decreases it with white or green background')

parser.add_argument('-d', '--compute-diff', action='store_true',
                    help='compute luminance between 2 colors or more ; the first color represents the background, the others the foreground')

parser.add_argument('-F', '--output-format', default='ansi', choices=['ansi', 'html'])
parser.add_argument('-T', '--html-title', help='title of html page when --output-format=html')

parser.add_argument('themes_or_colors', metavar='THEME_OR_COLOR', nargs='+',
                    help='a .theme file or a color (#rgb, #rrggbb, #argb, #aarrggbb) when -d / --compute-diff is used')

args = parser.parse_intermixed_args()


RGBColor = tuple[int, int, int]

def parse_rgb_color(color: str, bg: RGBColor) -> RGBColor:
    n = len(color)

    if not color.startswith('#') or n not in (7, 9, 4, 5):
        raise Exception(f'Invalid argb format: {color}')

    try:
        argb = int(color[1:], 16)
    except ValueError:
        raise Exception(f'Invalid argb format: {color}')

    # format: #rrggbb or #aarrggbb
    if n == 7 or n == 9:
        result = (
            (argb >> 16) & 0xff,
            (argb >>  8) & 0xff,
            (argb      ) & 0xff,
        )
        if n == 7:
            return result
        alpha = argb >> 24

    # format: #rgb or #argb
    else:
        (r, g, b) = (
            (argb >> 8) & 0xf,
            (argb >> 4) & 0xf,
            (argb     ) & 0xf,
        )
        result = ((r << 4 | r), (g << 4 | g), (b << 4 | b))
        if n == 4:
            return result
        alpha = argb >> 12
        alpha |= alpha << 4

    # alpha blend
    return (
        (alpha * result[0] + (255 - alpha) * bg[0]) // 255,
        (alpha * result[1] + (255 - alpha) * bg[1]) // 255,
        (alpha * result[2] + (255 - alpha) * bg[2]) // 255,
    )


# based on https://drafts.csswg.org/css-color/#color-conversion-code (CSS 4)
# 17. Sample code for Color Conversions

def lin_sRGB(c: int) -> float:
    # [0, 255] to [0, 1]
    v = c / 255.0
    if v <= 0.04045:
        return v / 12.92
    return ((v + 0.055) / 1.055) ** 2.4

sRGB_to_Y_mat = (
    0.21263900587151036,
    0.71516867876775592,
    0.072192315360733714,
)

DisplayP3_to_Y_mat = (
    0.22897456406974884,
    0.69173852183650619,
    0.079286914093744998,
)

# not in CSS
Okl_to_Y_mat = (
    # These values are the formula which calculates `l` in the XYZ to Okalab transformation.
    # (https://bottosson.github.io/posts/oklab/#converting-from-linear-srgb-to-oklab)
    # Specifically taking these values doesn't really make sense, but compared to sRGB
    # - the luminance between red and green will be very greatly decreased
    # - the luminance between red and white will be decreased
    # - the luminance between red and blue / black color will be greatly increased
    # - the luminance between blue and green will be decreased
    0.4122214708,
    0.5363325363,
    0.0514459929,
)

def lin_AdobeRGB(c: int) -> float:
    # [0, 255] to [0, 1]
    return (c / 255.0) ** 2.19921875

AdobeRGB_to_Y_mat = (
    0.29734497525053616,
    0.62736356625546597,
    0.07529145849399789,
)

def lin_Rec2020(c: int) -> float:
    # [0, 255] to [0, 1]
    v = c / 255.0

    if v < 0.08124285829863151:
        return v / 4.5

    return ((v + 0.09929682680944) / 1.09929682680944) ** (1 / 0.45)

Rec2020_to_Y_mat = (
    0.26270021201126703,
    0.67799807151887104,
    0.059301716469861945,
)

def make_to_Y(lin, mat):
    def to_Y(rgb: RGBColor) -> float:
        return (
            mat[0] * lin(rgb[0]) +
            mat[1] * lin(rgb[1]) +
            mat[2] * lin(rgb[2])
        )
    return to_Y


if args.color_space == 'sRGB':
    rgb_to_Y = make_to_Y(lin_sRGB, sRGB_to_Y_mat)
elif args.color_space == 'Okl':
    rgb_to_Y = make_to_Y(lin_sRGB, Okl_to_Y_mat)
elif args.color_space == 'DisplayP3':
    rgb_to_Y = make_to_Y(lin_sRGB, DisplayP3_to_Y_mat)
elif args.color_space == 'AdobeRGB':
    rgb_to_Y = make_to_Y(lin_AdobeRGB, AdobeRGB_to_Y_mat)
else:  # Rec2020
    rgb_to_Y = make_to_Y(lin_Rec2020, Rec2020_to_Y_mat)


# https://github.com/Myndex/apca-w3

# G-4g constants for use with 2.4 exponent
normBG = 0.56
normTXT = 0.57
revTXT = 0.62
revBG = 0.65

# G-4g Clamps and Scalers
blkThrs = 0.022
blkClmp = 1.414
scaleBoW = 1.14
scaleWoB = 1.14
loBoWoffset = 0.027
loWoBoffset = 0.027
deltaYmin = 0.0005
loClip = 0.0  # originally 0.1, but this limits the contrast to 7.5

def APCA_contrast(txtY: float, bgY: float) -> float:
    ## BLACK SOFT CLAMP

    # Soft clamps Y for either color if it is near black.

    if txtY <= blkThrs:
        txtY += (blkThrs - txtY) ** blkClmp

    if bgY <= blkThrs:
        bgY += (blkThrs - bgY) ** blkClmp

    # Return 0 Early for extremely low ∆Y
    if abs(bgY - txtY) < deltaYmin:
        return 0.0

    ## APCA/SAPC CONTRAST - LOW CLIP (W3 LICENSE)

    # For normal polarity, black text on white (BoW)
    # Calculate the SAPC contrast value and scale
    if bgY > txtY:
        SAPC = (bgY ** normBG - txtY ** normTXT) * scaleBoW

        # Low Contrast smooth rollout to prevent polarity reversal
        # and also a low-clip for very low contrasts
        outputContrast = 0.0 if SAPC < loClip else SAPC - loBoWoffset

    # For reverse polarity, light text on dark (WoB)
    # WoB should always return negative value.
    else:
        SAPC = (bgY ** revBG - txtY ** revTXT) * scaleWoB

        outputContrast = 0.0 if SAPC > -loClip else SAPC + loWoBoffset

    return outputContrast * 100.0


class ColorInfo:
    text: str = '#000'
    color: RGBColor = (0, 0, 0)
    Y: float = 0

    def __init__(self, rgb: str, bg: RGBColor):
        self.text = rgb
        self.color = parse_rgb_color(rgb, bg)
        self.Y = rgb_to_Y(self.color)

    def __str__(self) -> str:
        return self.text


NORMAL_LUMINANCE     = (90, 75, 60, 45, 35)
BOLD_LUMINANCE       = (80, 65, 50, 38, 30)
SPELL_LUMINANCE      = (70, 55, 40, 30, 25)
DECORATION_LUMINANCE = (60, 45, 30, 15, 10)

if args.scores:
    scores_update_len = min(20, len(args.scores))
    scores = [*NORMAL_LUMINANCE, *BOLD_LUMINANCE, *SPELL_LUMINANCE, *DECORATION_LUMINANCE]
    scores[:scores_update_len] = args.scores[:scores_update_len]
    NORMAL_LUMINANCE = scores[0:5]
    BOLD_LUMINANCE = scores[5:10]
    SPELL_LUMINANCE = scores[10:15]
    DECORATION_LUMINANCE = scores[15:20]

BOLD_TEXT = (BOLD_LUMINANCE, True, 'Text. ▐')
NORMAL_TEXT = (NORMAL_LUMINANCE, False, 'Text. ▐')
DECORATION_TEXT = (DECORATION_LUMINANCE, False, 'Text. ▐')
HEADER = (
    '\x1b[35m'
    ' Background                             |'
    ' Foreground                                  |'
    f' {(args.add_luminance or args.add_percent_luminance) and "   Luminance  " or " Lum "} |'
    ' Score\x1b[m'
)

RANK1 = '\x1b[32m'
RANK2 = '\x1b[32m'
RANK3 = '\x1b[32m'
RANK4 = '\x1b[33m'
RANK5 = '\x1b[31m'
RANK6 = '\x1b[31;1m'

def ffloat(x: float) -> str:
    s = f'{x:>5.1f}\x1b[m'
    return s.replace('.', '\x1b[37m.')

def flum(luminance: float,
         add_luminance: float,
         add_percent_luminance: float,
         bg: ColorInfo,
         fg: ColorInfo,
         luminance_values: tuple[int, int, int, int, int],
         is_bold: bool,
         sample_text: str
         ) -> str:
    """
    Compute and formate a score
    """
    luminance = abs(luminance)
    adjusted_luminance = luminance

    adjusted_text = ''
    if add_luminance or add_percent_luminance:
        adjusted_luminance += adjusted_luminance * add_percent_luminance + add_luminance
        adjusted_luminance = max(0, min(108, adjusted_luminance))
        adjusted_text = f' \x1b[35m->\x1b[m {ffloat(adjusted_luminance)}'

    (AAAA, AAA, AA, BAD, VERY_BAD) = luminance_values
    if adjusted_luminance >= AAAA:
        score = f'{RANK1}AAAA'
    elif adjusted_luminance >= AAA:
        score = f'{RANK2}AAA '
    elif adjusted_luminance >= AA:
        score = f'{RANK3}AA  '
    elif adjusted_luminance >= BAD:
        score = f'{RANK4}A   '
    elif adjusted_luminance >= VERY_BAD:
        score = f'{RANK5}FAIL'
    else:
        score = f'{RANK6}FAIL'

    (r1, g1, b1) = fg.color
    (r2, g2, b2) = bg.color
    bold = ';1' if is_bold else ''
    return f'{ffloat(luminance)}{adjusted_text} | {score}\x1b[m  ' \
           f'\x1b[38;2;{r1};{g1};{b1};48;2;{r2};{g2};{b2}{bold}m {sample_text} \x1b[m'

def color2ansi(rgb: RGBColor) -> str:
    return f'{rgb[0]};{rgb[1]};{rgb[2]}'

spaces = '                                                                    '
def fcol_impl(name: str, rgb: str, n: int) -> str:
    w = spaces[0: n - (len(name) + len(rgb) + 3)]
    return f'{name} \x1b[37m({rgb})\x1b[m{w}'

def fcol1(name: str, rgb: str) -> str:
    return fcol_impl(name, rgb, 38)

def fcol2(name: str, rgb: str) -> str:
    return fcol_impl(name, rgb, 43)

def create_tab_from_text_styles(min_luminance: float,
                                max_luminance: float,
                                add_luminance: float,
                                add_percent_luminance: float,
                                col1: str,
                                kstyle: str,
                                bg: ColorInfo,
                                text_styles: dict[str, dict[str, str | bool]]
                                ) -> str:
    lines = []
    for name, defs in sorted(text_styles.items()):
        if style := defs.get(kstyle):
            fg = ColorInfo(style, bg.color)
            lum = APCA_contrast(fg.Y, bg.Y)
            if min_luminance <= abs(lum) <= max_luminance:
                bold = defs.get('bold', False)
                result = flum(lum, add_luminance, add_percent_luminance,
                              bg, fg, *(BOLD_TEXT if bold else NORMAL_TEXT))
                lines.append(f' {col1} | {fcol2(name, fg.text)} | {result}')
    return '\n'.join(lines)


output = []

def run_borders(
    min_luminance: float,
    max_luminance: float,
    add_luminance: float,
    add_percent_luminance: float,
    editor_colors: dict[str, str],
    bg_editor: RGBColor
) -> None:
    output.append('\n\x1b[34mIcon Border\x1b[m:\n')

    bg_icon = ColorInfo(editor_colors['IconBorder'], (0, 0, 0))
    bg_current_line = ColorInfo(editor_colors['CurrentLine'], bg_editor.color)
    bg_current_line_border = ColorInfo(editor_colors['CurrentLine'], bg_icon.color)

    fg_line = ColorInfo(editor_colors['LineNumbers'], bg_icon.color)
    fg_current = ColorInfo(editor_colors['CurrentLineNumber'], bg_icon.color)
    fg_separator = ColorInfo(editor_colors['Separator'], bg_icon.color)
    fg_modified = ColorInfo(editor_colors['ModifiedLines'], bg_icon.color)
    fg_saved = ColorInfo(editor_colors['SavedLines'], bg_icon.color)

    xbg = color2ansi(bg_icon.color)
    xborder = f'\x1b[38;2;{color2ansi(fg_line.color)};48;2;{xbg}m'
    xsaved = f'\x1b[48;2;{color2ansi(fg_saved.color)};38;2;{xbg}m▋{xborder}'
    xmodified = f'\x1b[48;2;{color2ansi(fg_modified.color)};38;2;{xbg}m▋{xborder}'
    xeditor = f'\x1b[38;2;{color2ansi(fg_separator.color)}m▕\x1b[m'\
              f'\x1b[48;2;{color2ansi(bg_editor.color)}m        \x1b[m'

    cbg = color2ansi(bg_current_line_border.color)
    cborder = f'\x1b[38;2;{color2ansi(fg_current.color)};48;2;{cbg}m'
    csaved = f'\x1b[48;2;{color2ansi(fg_saved.color)};38;2;{cbg}m▋{cborder}'
    cmodified = f'\x1b[48;2;{color2ansi(fg_modified.color)};38;2;{cbg}m▋{cborder}'
    ceditor = f'\x1b[38;2;{color2ansi(fg_separator.color)}m▕\x1b[m'\
              f'\x1b[48;2;{color2ansi(bg_current_line.color)}m        \x1b[m'

    # imitates the border of Kate editor
    output.append(f'          LineNumbers: {xborder} 42 {xeditor} bg:  IconBorder | BackgroundColor\n'
          f'    CurrentLineNumber: {cborder} 43 {ceditor} bg: CurrentLine | CurrentLine\n'
          f'          LineNumbers: {xborder} 44{xmodified}{xeditor} (ModifiedLines)\n'
          f'    CurrentLineNumber: {cborder} 45{cmodified}{ceditor}\n'
          f'          LineNumbers: {xborder} 46{xsaved}{xeditor} (SavedLines)\n'
          f'    CurrentLineNumber: {cborder} 47{csaved}{ceditor}\n'
          f'                            ⧹ Separator\n\n{HEADER}\n')

    color_line_number = ('LineNumbers', fg_line, NORMAL_TEXT)
    colors = (
        ('CurrentLineNumber', fg_current, NORMAL_TEXT),
        ('ModifiedLines', fg_modified, DECORATION_TEXT),
        ('SavedLines', fg_saved, DECORATION_TEXT),
    )

    for name, bg, colors in (
        ('IconBorder', bg_icon, (color_line_number, *colors)),
        ('CurrentLine', bg_current_line_border, colors),
    ):
        col = fcol1(name, bg.text)
        lines = []
        for k, fg, text_data in colors:
            lum = APCA_contrast(fg.Y, bg.Y)
            if min_luminance <= abs(lum) <= max_luminance:
                result = flum(lum, add_luminance, add_percent_luminance,
                              bg, fg, *text_data)
                lines.append(f' {col} | {fcol2(k, fg.text)} | {result}')

        if lines:
            output.append('\n'.join(lines))
            output.append('\n\n')

    # table for Separator color
    for name, bg in (
        ('IconBorder', bg_icon),
        ('CurrentLine', bg_current_line_border),
        ('BackgroundColor', bg_editor),
    ):
        lum = APCA_contrast(fg_separator.Y, bg.Y)
        if min_luminance <= abs(lum) <= max_luminance:
            col = fcol1(name, bg.text)
            result = flum(lum, add_luminance, add_percent_luminance,
                          bg, fg_separator, DECORATION_LUMINANCE, False, NORMAL_TEXT[2])
            output.append(f' {col} | {fcol2("Separator", fg_separator.text)} | {result}\n')


def run(d: dict[str, str | dict[str, bool | str | dict[str, bool | str]]],
        min_luminance: float,
        max_luminance: float,
        add_luminance: float,
        add_percent_luminance: float,
        show_borders: bool,
        show_custom_styles: bool,
        show_standard_styles: bool,
        accepted_backgrounds: set[str] | None,
        accepted_languages: set[str] | None
        ) -> None:
    editor_colors = d['editor-colors']

    bg_editor = ColorInfo(editor_colors['BackgroundColor'], (0, 0, 0))

    output.append(f'\x1b[34;1mTheme\x1b[m: {d["metadata"]["name"]}\n')

    if show_borders:
        run_borders(min_luminance, max_luminance,
                    add_luminance, add_percent_luminance,
                    editor_colors, bg_editor)

    #
    # Editor
    #

    output.append('\n\x1b[34mText Area\x1b[m:\n')

    editor_bg_colors = {
        k: (ColorInfo(editor_colors[k], bg_editor.color), 'text-color')
        for k in (
            'TemplateReadOnlyPlaceholder',
            'TemplatePlaceholder',
            'TemplateFocusedPlaceholder',
            'TemplateBackground',
            'MarkBookmark',
            'CodeFolding',
            'ReplaceHighlight',
            'SearchHighlight',
            'BracketMatching',
        )
        if not accepted_backgrounds or k in accepted_backgrounds
    }
    if not accepted_backgrounds or 'TextSelection' in accepted_backgrounds:
        editor_bg_colors['TextSelection'] = (
            ColorInfo(editor_colors['TextSelection'], bg_editor.color),
            'selected-text-color'
        )
    if not accepted_backgrounds or 'BackgroundColor' in accepted_backgrounds:
        editor_bg_colors['BackgroundColor'] = (bg_editor, 'text-color')

    text_styles = d['text-styles']
    custom_styles = d.get('custom-styles', {}) if show_custom_styles else {}

    for name, (bg, kstyle) in editor_bg_colors.items():
        col = fcol1(name, bg.text)

        if show_standard_styles:
            tab = create_tab_from_text_styles(
                min_luminance, max_luminance,
                add_luminance, add_percent_luminance,
                col, kstyle, bg, text_styles
            )

            #
            # Spell decoration
            #
            name = 'SpellChecking'
            fg = ColorInfo(editor_colors[name], bg_editor.color)
            lum = APCA_contrast(fg.Y, bg.Y)
            if min_luminance <= abs(lum) <= max_luminance:
                result = flum(lum, add_luminance, add_percent_luminance,
                              bg, fg, SPELL_LUMINANCE, False, '~~~~~~~')
                spell_line = f' {col} | {fcol2(name, fg.text)} | {result}'
                tab = f'{tab}\n{spell_line}' if tab else spell_line

            if tab:
                output.append(f'\n{HEADER}\n{tab}\n')

        # table by language for custom styles
        for language, defs in sorted(custom_styles.items()):
            if accepted_languages and language not in accepted_languages:
                continue
            if tab := create_tab_from_text_styles(
                min_luminance, max_luminance,
                add_luminance, add_percent_luminance,
                col, kstyle, bg, defs
            ):
                output.append(f'\n\x1b[36mLanguage: "{language}"\x1b[m:\n{tab}\n')

    # ignored:
    # - WordWrapMarker
    # - TabMarker
    # - IndentationLine
    # - MarkBreakpointActive
    # - MarkBreakpointReached
    # - MarkBreakpointDisabled
    # - MarkExecution
    # - MarkWarning
    # - MarkError

def result_legend(AAAA: float, AAA: float, AA: float, BAD: float, VERY_BAD: float) -> str:
    return (
        f'{RANK1}AAAA\x1b[m (>={AAAA}) ; '
        f'{RANK2}AAA\x1b[m (>={AAA}) ; '
        f'{RANK3}AA\x1b[m (>={AA}) ; '
        f'{RANK4}A\x1b[m (>={BAD}) ; '
        f'{RANK5}FAIL\x1b[m (>={VERY_BAD}) ; '
        f'{RANK6}FAIL\x1b[m (<{VERY_BAD})'
    )


if not args.no_legend:
    output.append(f'''Luminance legend:
- Range for light theme: [0; 106]
- Range for  dark theme: [0; 108]
- Result for normal text:    {result_legend(*NORMAL_LUMINANCE)}
- Result for bold text:      {result_legend(*BOLD_LUMINANCE)}
- Result for spelling error: {result_legend(*SPELL_LUMINANCE)}
- Result for decoration:     {result_legend(*DECORATION_LUMINANCE)}

Luminance adjustement: {args.add_percent_luminance:+}% {args.add_luminance:+}  (see -p and -a)

Color space: {args.color_space}

''')

add_luminance = args.add_luminance
add_percent_luminance = args.add_percent_luminance / 100

if args.compute_diff:
    bg = ColorInfo(args.themes_or_colors[0], (0,0,0))
    output.append('Background | Foreground\n')

    # compares the background with all foreground colors in normal and bold
    for color in args.themes_or_colors[1:]:
        fg = ColorInfo(color, bg.color)
        lum = APCA_contrast(fg.Y, bg.Y)
        col = f'{bg.text:^10} | {fg.text:^10} | '
        output.append(col)
        output.append(flum(lum, add_luminance, add_percent_luminance, bg, fg, *NORMAL_TEXT))
        output.append('\n')
        output.append(col)
        output.append(flum(lum, add_luminance, add_percent_luminance, bg, fg, *BOLD_TEXT))
        output.append('\n')
else:
    add_new_line = False
    for theme in args.themes_or_colors:
        if add_new_line:
            output.append('\n\n')
        add_new_line = True

        # read json theme file
        try:
            if theme == '-':
                data = json.load(sys.stdin)
            else:
                with open(theme) as f:
                    data = json.load(f)
        except OSError as e:
            print(f'\x1b[31m{e}\x1b[m', file=sys.stderr)
            continue

        run(data,
            args.min_luminance,
            args.max_luminance,
            add_luminance,
            add_percent_luminance,
            not args.no_borders,
            not args.no_custom_styles,
            not args.no_standard_styles,
            args.bg and set(args.bg),
            args.language and set(args.language),
        )

is_html = args.output_format == 'html' and not args.compute_diff

output = ''.join(output)

if is_html:
    import re

    ansi_to_html = {
        '1': 'bold',
        '31': 'red',
        '32': 'green',
        '33': 'orange',
        '34': 'blue',
        '35': 'purple',
        '36': 'cyan',
        '37': 'gray',
    }

    extract_color = re.compile(r'([34])8;2;(\d+);(\d+);(\d+)')
    extract_effect = re.compile(r'\d+')

    depth = 0
    def replace_styles(m) -> str:
        global depth

        effects = m[1]
        if not effects:
            ret = '</span>' * depth
            depth = 0
            return ret

        depth += 1
        colors = []
        def rgb(m) -> str:
           prop = 'color' if m[1] == '3' else 'background'
           colors.append(f'{prop}:rgb({m[2]},{m[3]},{m[4]})')
           return ''
        effects = extract_color.sub(rgb, effects)
        if colors:
            styles = ';'.join(colors)
            styles = f' style="{styles}"'
        else:
            styles = ''

        classes = ' '.join(map(lambda s: ansi_to_html[s], extract_effect.findall(effects)))
        if classes:
            classes = f' class="{classes}"'

        return f'<span{styles}{classes}>'

    output = re.sub(r'\x1b\[([^m]*)m', replace_styles, output)

try:
    if is_html:
        bg = data['editor-colors']['BackgroundColor'];
        rgb = parse_rgb_color(bg, (0,0,0))
        if (rgb[0] < 127) + (rgb[0] < 127) + (rgb[0] < 127) >= 2:
            tmode1 = '#light:target'
            tmode2 = ''
            mode1 = 'dark'
            mode2 = 'light'
        else:
            tmode1 = ''
            tmode2 = '#dark:target'
            mode1 = 'light'
            mode2 = 'dark'
        title = args.html_title or data["metadata"]["name"]
        sys.stdout.write(f'''<!DOCTYPE html>
<html><head><title>{title}</title><style>
html, body, #mode {{
  padding: 0;
  margin: 0;
}}

body {{
  padding: .5em;
}}

pre {{
  font-family: "JetBrains Mono", "Liberation Mono", Firacode, "DejaVu Sans Mono", Inconsolata, monospace;
}}

.bold {{ font-weight: bold }}

/* light theme */

body{tmode1} {{
  background: #ddd;
  color: #000;
}}

{tmode1} .red {{ color: #A02222 }}
{tmode1} .green {{ color: #229022 }}
{tmode1} .orange {{ color: #909022 }}
{tmode1} .blue {{ color: #2222A0 }}
{tmode1} .purple {{ color: #A022A0 }}
{tmode1} .cyan {{ color: #22A0A0 }}
{tmode1} .gray {{ color: Gray }}

/* dark theme */

body{tmode2} {{
  background: #222;
  color: #eee;
}}
{tmode2} .red {{ color: #D95555 }}
{tmode2} .green {{ color: #55D055 }}
{tmode2} .orange {{ color: #D0D055 }}
{tmode2} .blue {{ color: #68A0E8 }}
{tmode2} .purple {{ color: #D077D0 }}
{tmode2} .cyan {{ color: Turquoise }}
{tmode2} .gray {{ color: Gray }}

div {{
  position: absolute;
  top: -1px;
  left: 0;
}}

#mode a {{
  padding: .5rem 1rem;
}}

#light-mode {{
  color: #2222A0;
  background: #ddd;
}}
#light-mode:hover, #light-mode:focus {{ background: #ccc; }}

#dark-mode {{
  color: #68a0E8;
  background: #222;
}}
#dark-mode:hover, #dark-mode:focus {{ background: #333; }}

#{mode1}-mode {{ display: none }}
#{mode2}-mode {{ display: inline-block }}
#{mode2}:target #{mode2}-mode {{ display: none }}
#{mode2}:target #{mode1}-mode {{ display: inline-block }}

</style></head><body id="{mode2}">
<p id="mode"><a id="{mode2}-mode" href="#{mode2}">Switch to {mode2} mode</a><a id="{mode1}-mode" href="#{mode1}">Switch to {mode1} mode</a></p>
<pre>''')
        sys.stdout.write(output)
        sys.stdout.write('</pre></body></html>')
    else:
        sys.stdout.write(output)

    # flush output here to force SIGPIPE to be triggered
    sys.stdout.flush()
# open in `less` then closing can cause this error
except BrokenPipeError:
    # Python flushes standard streams on exit; redirect remaining output
    # to devnull to avoid another BrokenPipeError at shutdown
    import os
    devnull = os.open(os.devnull, os.O_WRONLY)
    os.dup2(devnull, sys.stdout.fileno())
