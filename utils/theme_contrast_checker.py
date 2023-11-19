#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2023 Jonathan Poelen <jonathan.poelen@gmail.com>
# SPDX-License-Identifier: MIT

import argparse
import json
import sys

parser = argparse.ArgumentParser(description='Contrast checker for themes.')

parser.add_argument('-f', '--bg', metavar='BACKGROUND', action='append',
                    help='Show only the specified background color styles')
parser.add_argument('-l', '--language', metavar='LANGUAGE', action='append',
                    help='Show only the specified language')

parser.add_argument('-c', '--no-custom-styles', action='store_true',
                    help='Do not display custom languages')
parser.add_argument('-s', '--no-standard-styles', action='store_true',
                    help='Do not display standard colors')
parser.add_argument('-b', '--no-borders', action='store_true',
                    help='Do not display border colors')
parser.add_argument('-H', '--no-legend', action='store_true',
                    help='Do not display legend')

parser.add_argument('-M', '--min-luminance', metavar='LUMINANCE', type=float, default=0,
                    help='Only displays colors with a lower luminance')
parser.add_argument('-L', '--max-luminance', metavar='LUMINANCE', type=float, default=110.0,
                    help='Only displays colors with a lower luminance')

parser.add_argument('-a', '--add-luminance', metavar='LUMINANCE', type=float, default=0,
                    help='Add fixed value for luminance')
parser.add_argument('-p', '--add-percent-luminance', metavar='LUMINANCE', type=float, default=0,
                    help='Add percent luminance. Apply before --add-luminance.')

parser.add_argument('-P', '--profile', default='Oklab',
                    choices=['Oklab', 'W3', 'DisplayP3', 'AdobeRGB'],
                    help='Select an APCA constant profile')

parser.add_argument('-d', '--compute-diff', action='store_true',
                    help='Compute luminance between 2 colors or more. The first color represents the background, the others the foreground')

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


if args.profile == 'Oklab':
    def inverse_gamma_companding(c: int) -> float:
        # [0, 255] to [0, 1]
        v = c / 255.0;
        if v <= 0.04045:
            return v / 12.92
        return ((v + 0.055) / 1.055) ** 2.4

    def sRGBtoY(rgb: RGBColor) -> float:
        return (
            0.4122214708 * inverse_gamma_companding(rgb[0]) +
            0.5363325363 * inverse_gamma_companding(rgb[1]) +
            0.0514459929 * inverse_gamma_companding(rgb[2])
        )
else:
    if args.profile == 'W3':
        mainTRC = 2.4  # exponent for emulating actual monitor perception
        # sRGB coefficients
        sRco = 0.2126729
        sGco = 0.7151522
        sBco = 0.0721750

    elif args.profile == 'DisplayP3':
        mainTRC = 2.4  # exponent for emulating actual monitor perception
        # displayP3 coefficients
        sRco = 0.2289829594805780
        sGco = 0.6917492625852380
        sBco = 0.0792677779341829

    else:  # AdobeRGB
        mainTRC = 2.35  # exponent for emulating actual monitor perception
        # adobeRGB coefficients
        sRco = 0.2973550227113810
        sGco = 0.6273727497145280
        sBco = 0.0752722275740913

    def sRGBtoY(rgb: RGBColor) -> float:
        return (
            sRco * (rgb[0] / 255) ** mainTRC +
            sGco * (rgb[1] / 255) ** mainTRC +
            sBco * (rgb[2] / 255) ** mainTRC
        )

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
        self.Y = sRGBtoY(self.color)

    def __str__(self) -> str:
        return self.text


NORMAL_LUMINANCE     = (90, 75, 60, 45, 35)
BOLD_LUMINANCE       = (80, 65, 50, 38, 30)
SPELL_LUMINANCE      = (70, 55, 40, 30, 25)
DECORATION_LUMINANCE = (60, 45, 30, 15, 10)

BOLD_TEXT = (BOLD_LUMINANCE, '\x1b[1mText. ▐')
NORMAL_TEXT = (NORMAL_LUMINANCE, 'Text. ▐')
DECORATION_TEXT = (DECORATION_LUMINANCE, NORMAL_TEXT[1])
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

def _ffloat(x: float) -> str:
    s = f'{x:>5.1f}\x1b[m'
    return s.replace('.', '\x1b[37m.')

def flum(luminance: float,
         add_luminance: float,
         add_percent_luminance: float,
         bg: ColorInfo,
         fg: ColorInfo,
         luminance_values: tuple[int, int, int, int, int],
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
        adjusted_text = f' \x1b[35m->\x1b[m {_ffloat(adjusted_luminance)}'

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
    return f'{_ffloat(luminance)}{adjusted_text} | {score}\x1b[m  ' \
           f'\x1b[38;2;{r1};{g1};{b1};48;2;{r2};{g2};{b2}m {sample_text} \x1b[m'

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


def run_borders(
    min_luminance: float,
    max_luminance: float,
    add_luminance: float,
    add_percent_luminance: float,
    editor_colors: dict[str, str],
    bg_editor: RGBColor
) -> None:
    print('\n\x1b[34mIcon Border\x1b[m:')

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
    print(f'          LineNumbers: {xborder} 42 {xeditor} bg:  IconBorder | BackgroundColor\n'
          f'    CurrentLineNumber: {cborder} 43 {ceditor} bg: CurrentLine | CurrentLine\n'
          f'          LineNumbers: {xborder} 44{xmodified}{xeditor} (ModifiedLines)\n'
          f'    CurrentLineNumber: {cborder} 45{cmodified}{ceditor}\n'
          f'          LineNumbers: {xborder} 46{xsaved}{xeditor} (SavedLines)\n'
          f'    CurrentLineNumber: {cborder} 47{csaved}{ceditor}\n'
          f'                            ⧹ Separator\n\n{HEADER}')

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
            print('\n'.join(lines))
            print()

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
                          bg, fg_separator, DECORATION_LUMINANCE, NORMAL_TEXT[1])
            print(f' {col} | {fcol2("Separator", fg_separator.text)} | {result}')


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

    print(f'\x1b[1;34mTheme\x1b[m: {d["metadata"]["name"]}')

    if show_borders:
        run_borders(min_luminance, max_luminance,
                    add_luminance, add_percent_luminance,
                    editor_colors, bg_editor)

    #
    # Editor
    #

    print('\n\x1b[34mText Area\x1b[m:')

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
                              bg, fg, SPELL_LUMINANCE, '~~~~~~~')
                spell_line = f' {col} | {fcol2(name, fg.text)} | {result}'
                tab = f'{tab}\n{spell_line}' if tab else spell_line

            if tab:
                print(f'\n{HEADER}\n{tab}')

        # table by language for custom styles
        for language, defs in sorted(custom_styles.items()):
            if accepted_languages and language not in accepted_languages:
                continue
            if tab := create_tab_from_text_styles(
                min_luminance, max_luminance,
                add_luminance, add_percent_luminance,
                col, kstyle, bg, defs
            ):
                print(f'\n\x1b[36mLanguage: "{language}"\x1b[m:\n{tab}')

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


try:
    if not args.no_legend:
        print(f'''Luminance legend:
- Range for light theme: [0; 106]
- Range for  dark theme: [0; 108]
- Result for normal text:    {result_legend(*NORMAL_LUMINANCE)}
- Result for bold text:      {result_legend(*BOLD_LUMINANCE)}
- Result for spelling error: {result_legend(*SPELL_LUMINANCE)}
- Result for decoration:     {result_legend(*DECORATION_LUMINANCE)}

Luminance adjustement: {args.add_percent_luminance:+}% {args.add_luminance:+}  (see -p and -a)
''')

    add_luminance = args.add_luminance
    add_percent_luminance = args.add_percent_luminance / 100

    if args.compute_diff:
        bg = ColorInfo(args.themes_or_colors[0], (0,0,0))
        print('Background | Foreground')

        # compares the background with all foreground colors in normal and bold
        for color in args.themes_or_colors[1:]:
            fg = ColorInfo(color, bg.color)
            lum = APCA_contrast(fg.Y, bg.Y)
            col = f'{bg.text:^10} | {fg.text:^10} |'
            print(col, flum(lum, add_luminance, add_percent_luminance,
                            bg, fg, *NORMAL_TEXT))
            print(col, flum(lum, add_luminance, add_percent_luminance,
                            bg, fg, *BOLD_TEXT))
    else:
        add_new_line = False
        for theme in args.themes_or_colors:
            if add_new_line:
                print('\n')
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

    # flush output here to force SIGPIPE to be triggered
    sys.stdout.flush()
# open in `less` then closing can cause this error
except BrokenPipeError:
    # Python flushes standard streams on exit; redirect remaining output
    # to devnull to avoid another BrokenPipeError at shutdown
    import os
    devnull = os.open(os.devnull, os.O_WRONLY)
    os.dup2(devnull, sys.stdout.fileno())
