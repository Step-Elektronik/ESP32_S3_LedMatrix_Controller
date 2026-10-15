#!/usr/bin/env python3
"""Generate font-specific Turkish GFX glyphs for the complete font catalog.

The 48 GNU FreeFont variants are rasterized from their original Unicode TTF
glyphs using the same 141-DPI sizing used by Adafruit's fontconvert tool.  The
small bitmap fonts do not have matching TTF sources, so their accents are made
from their own period, comma, U/u and o shapes instead of a shared geometric
template.

GNU FreeFont source: https://ftp.gnu.org/gnu/freefont/freefont-ttf-20120503.zip
"""

from __future__ import annotations

import argparse
import math
import re
from dataclasses import dataclass
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


DPI = 141
PRIVATE_FIRST = 0x7F
TURKISH_CHARACTERS = (
    "Ç",
    "Ğ",
    "İ",
    "Ö",
    "Ş",
    "Ü",
    "ç",
    "ğ",
    "ı",
    "ö",
    "ş",
    "ü",
    "°",
)

FONT_NAMES = (
    "Apple4x6",
    "Apple5x7",
    "Apple6x10",
    "Apple8x13",
    "GohuFont6x11",
    "GohuFont6x11b",
    "Fixed",
    "FreeMono9pt7b",
    "FreeMono12pt7b",
    "FreeMono18pt7b",
    "FreeMono24pt7b",
    "FreeMonoBold9pt7b",
    "FreeMonoBold12pt7b",
    "FreeMonoBold18pt7b",
    "FreeMonoBold24pt7b",
    "FreeMonoBoldOblique9pt7b",
    "FreeMonoBoldOblique12pt7b",
    "FreeMonoBoldOblique18pt7b",
    "FreeMonoBoldOblique24pt7b",
    "FreeMonoOblique9pt7b",
    "FreeMonoOblique12pt7b",
    "FreeMonoOblique18pt7b",
    "FreeMonoOblique24pt7b",
    "FreeSans9pt7b",
    "FreeSans12pt7b",
    "FreeSans18pt7b",
    "FreeSans24pt7b",
    "FreeSansBold9pt7b",
    "FreeSansBold12pt7b",
    "FreeSansBold18pt7b",
    "FreeSansBold24pt7b",
    "FreeSansBoldOblique9pt7b",
    "FreeSansBoldOblique12pt7b",
    "FreeSansBoldOblique18pt7b",
    "FreeSansBoldOblique24pt7b",
    "FreeSansOblique9pt7b",
    "FreeSansOblique12pt7b",
    "FreeSansOblique18pt7b",
    "FreeSansOblique24pt7b",
    "FreeSerif9pt7b",
    "FreeSerif12pt7b",
    "FreeSerif18pt7b",
    "FreeSerif24pt7b",
    "FreeSerifBold9pt7b",
    "FreeSerifBold12pt7b",
    "FreeSerifBold18pt7b",
    "FreeSerifBold24pt7b",
    "FreeSerifBoldItalic9pt7b",
    "FreeSerifBoldItalic12pt7b",
    "FreeSerifBoldItalic18pt7b",
    "FreeSerifBoldItalic24pt7b",
    "FreeSerifItalic9pt7b",
    "FreeSerifItalic12pt7b",
    "FreeSerifItalic18pt7b",
    "FreeSerifItalic24pt7b",
    "Org_01",
    "Picopixel",
    "Tiny3x3a2pt7b",
    "TomThumb",
)

BITMAP_FONT_HEADERS = {
    "Apple4x6": "FontGfx_apple4x6.h",
    "Apple5x7": "FontGfx_apple5x7.h",
    "Apple6x10": "FontGfx_apple6x10.h",
    "Apple8x13": "FontGfx_apple8x13.h",
    "GohuFont6x11": "FontGfx_gohufont6x11.h",
    "GohuFont6x11b": "FontGfx_gohufont6x11b.h",
    "Fixed": "FontGfx_tom_thumb.h",
    "Org_01": "Org_01.h",
    "Picopixel": "Picopixel.h",
    "Tiny3x3a2pt7b": "Tiny3x3a2pt7b.h",
    "TomThumb": "TomThumb.h",
}

DECORATED_CHARACTERS = (
    ("Ç", "C", "cedilla"),
    ("Ğ", "G", "breve_upper"),
    ("İ", "I", "dot"),
    ("Ö", "O", "diaeresis"),
    ("Ş", "S", "cedilla"),
    ("Ü", "U", "diaeresis"),
    ("ç", "c", "cedilla"),
    ("ğ", "g", "breve_lower"),
    ("ı", "i", "dotless"),
    ("ö", "o", "diaeresis"),
    ("ş", "s", "cedilla"),
    ("ü", "u", "diaeresis"),
)


@dataclass
class Glyph:
    pixels: list[list[bool]]
    x_advance: int
    x_offset: int
    y_offset: int

    @property
    def width(self) -> int:
        return len(self.pixels[0]) if self.pixels else 0

    @property
    def height(self) -> int:
        return len(self.pixels)


def strip_comments(value: str) -> str:
    value = re.sub(r"/\*.*?\*/", "", value, flags=re.S)
    return re.sub(r"//[^\r\n]*", "", value)


def integer_tokens(value: str) -> list[int]:
    tokens = re.findall(r"-?0[xX][0-9a-fA-F]+|-?\d+", value)
    return [
        int(token, 16) if token.lower().lstrip("-").startswith("0x") else int(token, 10)
        for token in tokens
    ]


def parse_gfx_font(header_path: Path, symbol: str) -> dict[str, Glyph]:
    source = header_path.read_text(encoding="utf-8")
    bitmap_match = re.search(
        rf"\b{re.escape(symbol)}Bitmaps\s*\[\s*\]\s*PROGMEM\s*=\s*\{{(.*?)\}}\s*;",
        source,
        flags=re.S,
    )
    glyph_match = re.search(
        rf"\b{re.escape(symbol)}Glyphs\s*\[\s*\]\s*PROGMEM\s*=\s*\{{(.*?)\}}\s*;",
        source,
        flags=re.S,
    )
    font_match = re.search(
        rf"\b{re.escape(symbol)}\s+PROGMEM\s*=\s*\{{(.*?)\}}\s*;",
        source,
        flags=re.S,
    )
    if not bitmap_match or not glyph_match or not font_match:
        raise ValueError(f"Could not parse GFX font {symbol} from {header_path}")

    bitmap = integer_tokens(strip_comments(bitmap_match.group(1)))
    glyph_values = [
        integer_tokens(entry)
        for entry in re.findall(r"\{([^{}]+)\}", strip_comments(glyph_match.group(1)))
    ]
    glyph_values = [entry for entry in glyph_values if len(entry) == 6]
    font_values = integer_tokens(strip_comments(font_match.group(1)))
    if len(font_values) < 3:
        raise ValueError(f"Could not read character range for {symbol}")
    first = font_values[-3]
    last = font_values[-2]
    expected_glyph_count = last - first + 1
    if len(glyph_values) < expected_glyph_count:
        raise ValueError(
            f"Unexpected glyph count for {symbol}: {len(glyph_values)} < {expected_glyph_count}"
        )
    # TomThumb optionally appends a non-contiguous Unicode extension after
    # ASCII; the GFXfont range still deliberately ends at 0x7E.
    glyph_values = glyph_values[:expected_glyph_count]

    result: dict[str, Glyph] = {}
    for character_code, values in zip(range(first, last + 1), glyph_values):
        bitmap_offset, width, height, advance, x_offset, y_offset = values
        pixels: list[list[bool]] = []
        for y in range(height):
            row: list[bool] = []
            for x in range(width):
                bit_index = y * width + x
                byte_value = bitmap[bitmap_offset + bit_index // 8]
                row.append(bool(byte_value & (0x80 >> (bit_index & 7))))
            pixels.append(row)
        result[chr(character_code)] = Glyph(
            pixels, advance, x_offset, y_offset
        )
    return result


def trim_pixels(pixels: list[list[bool]]) -> list[list[bool]]:
    if not pixels or not pixels[0]:
        return []
    occupied = [
        (x, y)
        for y, row in enumerate(pixels)
        for x, value in enumerate(row)
        if value
    ]
    if not occupied:
        return []
    left = min(x for x, _ in occupied)
    right = max(x for x, _ in occupied)
    top = min(y for _, y in occupied)
    bottom = max(y for _, y in occupied)
    return [row[left : right + 1] for row in pixels[top : bottom + 1]]


def resize_pixels(
    pixels: list[list[bool]], target_width: int, target_height: int
) -> list[list[bool]]:
    source = trim_pixels(pixels)
    if not source or target_width <= 0 or target_height <= 0:
        return []
    source_height = len(source)
    source_width = len(source[0])
    return [
        [
            source[min(source_height - 1, (y * source_height) // target_height)][
                min(source_width - 1, (x * source_width) // target_width)
            ]
            for x in range(target_width)
        ]
        for y in range(target_height)
    ]


def glyph_points(glyph: Glyph) -> set[tuple[int, int]]:
    return {
        (glyph.x_offset + x, glyph.y_offset + y)
        for y, row in enumerate(glyph.pixels)
        for x, value in enumerate(row)
        if value
    }


def glyph_from_points(points: set[tuple[int, int]], x_advance: int) -> Glyph:
    if not points:
        return Glyph([], x_advance, 0, 0)
    left = min(x for x, _ in points)
    right = max(x for x, _ in points)
    top = min(y for _, y in points)
    bottom = max(y for _, y in points)
    pixels = [
        [(left + x, top + y) in points for x in range(right - left + 1)]
        for y in range(bottom - top + 1)
    ]
    return Glyph(pixels, x_advance, left, top)


def add_mark(
    points: set[tuple[int, int]],
    pixels: list[list[bool]],
    left: int,
    top: int,
) -> None:
    for y, row in enumerate(pixels):
        for x, value in enumerate(row):
            if value:
                points.add((left + x, top + y))


def centered_left(base: Glyph, mark_width: int) -> int:
    return base.x_offset + (base.width - mark_width) // 2


def remove_i_dot(base: Glyph) -> Glyph:
    separator = None
    ink_seen = False
    for y, row in enumerate(base.pixels[:-1]):
        if any(row):
            ink_seen = True
        elif ink_seen and any(any(later) for later in base.pixels[y + 1 :]):
            separator = y
            break
    if separator is None:
        separator = max(0, base.height // 4 - 1)
    points = {
        (base.x_offset + x, base.y_offset + y)
        for y, row in enumerate(base.pixels)
        if y > separator
        for x, value in enumerate(row)
        if value
    }
    return glyph_from_points(points, base.x_advance)


def breve_from_u(source: Glyph, maximum_width: int) -> list[list[bool]]:
    source_pixels = trim_pixels(source.pixels)
    source_height = len(source_pixels)
    section_height = max(2, min(3, math.ceil(source_height * 0.4)))
    mark = trim_pixels(source_pixels[-section_height:])
    if len(mark[0]) > maximum_width:
        target_height = max(2, round(len(mark) * maximum_width / len(mark[0])))
        mark = resize_pixels(mark, maximum_width, target_height)
    return mark


def degree_ring(diameter: int) -> list[list[bool]]:
    """Closed degree ring: a solid dot at 2 px, a square ring at 3 px and a
    rounded ring above that, so no arc is lost at small sizes."""
    if diameter <= 2:
        return [[True] * diameter for _ in range(diameter)]
    if diameter == 3:
        return [[True, True, True], [True, False, True], [True, True, True]]
    pixels = [[False] * diameter for _ in range(diameter)]
    for index in range(1, diameter - 1):
        pixels[0][index] = pixels[-1][index] = True
        pixels[index][0] = pixels[index][-1] = True
    return pixels


def bitmap_font_degree(glyphs: dict[str, Glyph]) -> Glyph:
    # Shrinking the font's own "o" dropped the right and bottom arcs and left
    # the sign at x-height. Draw a closed ring sized from the digit height and
    # align its top with the tallest digit or capital, as in "23°C".
    references = [glyphs[character] for character in "0123456789C"]
    cap_height = max(glyphs[character].height for character in "0123456789")
    top = min(glyph.y_offset for glyph in references)
    diameter = 2 if cap_height <= 3 else (3 if cap_height <= 6 else 4)
    digit_advance = glyphs["0"].x_advance
    x_offset = 1 if digit_advance - diameter >= 2 else 0
    return Glyph(degree_ring(diameter), x_offset + diameter + 1, x_offset, top)


def decorate_bitmap_font(glyphs: dict[str, Glyph]) -> list[Glyph]:
    dot = trim_pixels(glyphs["."].pixels)
    comma = trim_pixels(glyphs[","].pixels)
    generated: list[Glyph] = []

    for _, base_character, decoration in DECORATED_CHARACTERS:
        base = glyphs[base_character]
        if decoration == "dotless":
            generated.append(remove_i_dot(base))
            continue

        points = glyph_points(base)
        if decoration == "cedilla":
            mark = comma
            mark_left = centered_left(base, len(mark[0]))
            mark_top = base.y_offset + base.height
            add_mark(points, mark, mark_left, mark_top)
        elif decoration == "dot":
            mark_left = centered_left(base, len(dot[0]))
            mark_top = base.y_offset - len(dot) - 1
            add_mark(points, dot, mark_left, mark_top)
        elif decoration == "diaeresis":
            separation = 1
            mark_width = len(dot[0]) * 2 + separation
            mark_left = centered_left(base, mark_width)
            mark_top = base.y_offset - len(dot) - 1
            add_mark(points, dot, mark_left, mark_top)
            add_mark(points, dot, mark_left + len(dot[0]) + separation, mark_top)
        else:
            source = glyphs["U" if decoration == "breve_upper" else "u"]
            mark = breve_from_u(source, max(3, base.width))
            mark_left = centered_left(base, len(mark[0]))
            mark_top = base.y_offset - len(mark) - 1
            add_mark(points, mark, mark_left, mark_top)
        generated.append(glyph_from_points(points, base.x_advance))

    generated.append(bitmap_font_degree(glyphs))
    return generated


def freefont_configuration(font_name: str) -> tuple[str, int] | None:
    match = re.fullmatch(r"(Free(?:Mono|Sans|Serif)(?:Bold)?(?:Oblique|Italic)?)(9|12|18|24)pt7b", font_name)
    if not match:
        return None
    return match.group(1) + ".ttf", int(match.group(2))


def rasterize_ttf_glyph(font: ImageFont.FreeTypeFont, character: str) -> Glyph:
    mask, offset = font.getmask2(character, mode="1", anchor="ls")
    width, height = mask.size
    values = list(mask)
    untrimmed = [
        [bool(values[y * width + x]) for x in range(width)]
        for y in range(height)
    ]
    occupied = [
        (x, y)
        for y, row in enumerate(untrimmed)
        for x, value in enumerate(row)
        if value
    ]
    if occupied:
        left = min(x for x, _ in occupied)
        right = max(x for x, _ in occupied)
        top = min(y for _, y in occupied)
        bottom = max(y for _, y in occupied)
        pixels = [row[left : right + 1] for row in untrimmed[top : bottom + 1]]
    else:
        left = top = 0
        pixels = []
    return Glyph(
        pixels,
        int(font.getlength(character)),
        int(offset[0]) + left,
        int(offset[1]) + top + 1,
    )


def ttf_font(freefont_dir: Path, font_name: str) -> ImageFont.FreeTypeFont:
    configuration = freefont_configuration(font_name)
    if configuration is None:
        raise ValueError(f"Not a GNU FreeFont catalog entry: {font_name}")
    filename, point_size = configuration
    path = freefont_dir / filename
    if not path.is_file():
        raise FileNotFoundError(path)
    pixel_size = round(point_size * DPI / 72)
    return ImageFont.truetype(
        str(path), pixel_size, layout_engine=ImageFont.Layout.BASIC
    )


def verify_freefont_rasterization(font_directory: Path, freefont_dir: Path) -> None:
    sample_characters = "ACGIOSUcgiorsu"
    metric_total = 0
    metric_exact = 0
    bitmap_exact = 0
    first_mismatches: list[str] = []
    for font_name in FONT_NAMES:
        if freefont_configuration(font_name) is None:
            continue
        header_path = font_directory / f"{font_name}.h"
        if not header_path.is_file():
            continue
        expected_glyphs = parse_gfx_font(header_path, font_name)
        font = ttf_font(freefont_dir, font_name)
        for character in sample_characters:
            expected = expected_glyphs[character]
            actual = rasterize_ttf_glyph(font, character)
            metric_total += 1
            expected_metrics = (
                expected.width,
                expected.height,
                expected.x_advance,
                expected.x_offset,
                expected.y_offset,
            )
            actual_metrics = (
                actual.width,
                actual.height,
                actual.x_advance,
                actual.x_offset,
                actual.y_offset,
            )
            if expected_metrics == actual_metrics:
                metric_exact += 1
            elif len(first_mismatches) < 5:
                first_mismatches.append(
                    f"{font_name}/{character}: {expected_metrics} != {actual_metrics}"
                )
            if expected.pixels == actual.pixels:
                bitmap_exact += 1
    print(
        f"FreeFont calibration: metrics {metric_exact}/{metric_total} exact, "
        f"bitmaps {bitmap_exact}/{metric_total} exact"
    )
    for mismatch in first_mismatches:
        print(f"  warning: {mismatch}")


def pack_glyph(glyph: Glyph) -> list[int]:
    packed = [0] * ((glyph.width * glyph.height + 7) // 8)
    for y, row in enumerate(glyph.pixels):
        for x, value in enumerate(row):
            if value:
                bit_index = y * glyph.width + x
                packed[bit_index // 8] |= 0x80 >> (bit_index & 7)
    return packed


def format_bytes(values: list[int], indent: str = "  ") -> list[str]:
    return [
        indent + ", ".join(f"0x{value:02X}" for value in values[index : index + 12]) + ","
        for index in range(0, len(values), 12)
    ]


def generate_header(font_glyphs: dict[str, list[Glyph] | None]) -> str:
    lines = [
        "// Generated by tools/generate_turkish_font_extensions.py.",
        "// GNU FreeFont glyphs retain the GNU GPL + font embedding exception.",
        "// Do not edit this file by hand.",
        "",
        f"constexpr uint8_t turkishExtensionFirst = 0x{PRIVATE_FIRST:02X};",
        f"constexpr uint8_t turkishExtensionGlyphCount = {len(TURKISH_CHARACTERS)};",
        "",
    ]
    table_rows: list[str] = []
    for font_name in FONT_NAMES:
        glyphs = font_glyphs[font_name]
        if glyphs is None:
            table_rows.append("  {nullptr, nullptr},")
            continue
        bitmaps: list[int] = []
        glyph_rows: list[str] = []
        for index, (character, glyph) in enumerate(zip(TURKISH_CHARACTERS, glyphs)):
            offset = len(bitmaps)
            bitmaps.extend(pack_glyph(glyph))
            glyph_rows.append(
                "  {%d, %d, %d, %d, %d, %d},  // 0x%02X U+%04X %s"
                % (
                    offset,
                    glyph.width,
                    glyph.height,
                    glyph.x_advance,
                    glyph.x_offset,
                    glyph.y_offset,
                    PRIVATE_FIRST + index,
                    ord(character),
                    character,
                )
            )
        lines.append(f"const uint8_t {font_name}TurkishBitmaps[] PROGMEM = {{")
        lines.extend(format_bytes(bitmaps))
        lines.extend(("};", ""))
        lines.append(f"const TurkishExtensionGlyph {font_name}TurkishGlyphs[] PROGMEM = {{")
        lines.extend(glyph_rows)
        lines.extend(("};", ""))
        table_rows.append(
            f"  {{{font_name}TurkishBitmaps, {font_name}TurkishGlyphs}},"
        )

    lines.append("const TurkishExtensionFont turkishExtensionFonts[] PROGMEM = {")
    for font_name, row in zip(FONT_NAMES, table_rows):
        lines.append(f"{row}  // {font_name}")
    lines.extend(
        (
            "};",
            "",
            "static_assert(",
            "  sizeof(turkishExtensionFonts) / sizeof(turkishExtensionFonts[0]) == FontModel::fontCount,",
            '  "Turkish extension table must match the compiled font catalog");',
            "",
        )
    )
    return "\n".join(lines)


def validate_extensions(font_glyphs: dict[str, list[Glyph] | None]) -> None:
    if tuple(font_glyphs) != FONT_NAMES:
        raise ValueError("Generated font order no longer matches FontCatalog")
    signatures: set[tuple[object, ...]] = set()
    glyph_total = 0
    for font_name, glyphs in font_glyphs.items():
        if glyphs is None:
            if font_name != "Apple6x10":
                raise ValueError(f"Missing extension data for {font_name}")
            continue
        if len(glyphs) != len(TURKISH_CHARACTERS):
            raise ValueError(f"Incomplete Turkish range for {font_name}")
        signature: list[object] = []
        for character, glyph in zip(TURKISH_CHARACTERS, glyphs):
            if glyph.width <= 0 or glyph.height <= 0 or not any(map(any, glyph.pixels)):
                raise ValueError(f"Empty {character} glyph in {font_name}")
            if glyph.width > 255 or glyph.height > 255 or glyph.x_advance > 255:
                raise ValueError(f"Oversized {character} glyph in {font_name}")
            if not -128 <= glyph.x_offset <= 127 or not -128 <= glyph.y_offset <= 127:
                raise ValueError(f"Out-of-range {character} offset in {font_name}")
            signature.append(
                (
                    glyph.width,
                    glyph.height,
                    glyph.x_advance,
                    glyph.x_offset,
                    glyph.y_offset,
                    tuple(pack_glyph(glyph)),
                )
            )
            glyph_total += 1
        signatures.add(tuple(signature))
    print(
        f"Validated {glyph_total} generated glyphs; "
        f"{len(signatures)} distinct per-font Turkish sets"
    )


def render_preview(font_glyphs: dict[str, list[Glyph] | None], output: Path) -> None:
    apple_glyphs = parse_gfx_font(
        Path(__file__).resolve().parents[1]
        / "lib"
        / "Fonts"
        / BITMAP_FONT_HEADERS["Apple6x10"],
        "Apple6x10",
    )
    complete = dict(font_glyphs)
    complete["Apple6x10"] = [
        apple_glyphs[chr(PRIVATE_FIRST + index)]
        for index in range(len(TURKISH_CHARACTERS))
    ]

    row_layout: list[tuple[str, list[Glyph], int, int, int]] = []
    total_height = 20
    for font_name in FONT_NAMES:
        glyphs = complete[font_name]
        assert glyphs is not None
        configuration = freefont_configuration(font_name)
        point_size = configuration[1] if configuration else 0
        scale = 4 if configuration is None else (2 if point_size <= 12 else 1)
        top = min(glyph.y_offset for glyph in glyphs)
        bottom = max(glyph.y_offset + glyph.height for glyph in glyphs)
        row_height = max(32, (bottom - top + 3) * scale + 12)
        row_layout.append((font_name, glyphs, scale, top, row_height))
        total_height += row_height

    image = Image.new("RGB", (1120, total_height), "white")
    draw = ImageDraw.Draw(image)
    label_font = ImageFont.load_default(size=14)
    y = 10
    for font_name, glyphs, scale, top, row_height in row_layout:
        baseline = y + 6 - top * scale
        draw.text((10, y + 5), font_name, fill=(35, 43, 55), font=label_font)
        draw.line((250, baseline, 1110, baseline), fill=(222, 226, 232))
        cursor = 270
        for glyph in glyphs:
            for pixel_y, row in enumerate(glyph.pixels):
                for pixel_x, value in enumerate(row):
                    if not value:
                        continue
                    left = cursor + (glyph.x_offset + pixel_x) * scale
                    pixel_top = baseline + (glyph.y_offset + pixel_y) * scale
                    draw.rectangle(
                        (left, pixel_top, left + scale - 1, pixel_top + scale - 1),
                        fill=(15, 23, 42),
                    )
            cursor += glyph.x_advance * scale
        y += row_height
    output.parent.mkdir(parents=True, exist_ok=True)
    image.save(output)
    print(f"Preview -> {output}")


def main() -> None:
    project_root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--freefont-dir",
        required=True,
        type=Path,
        help="Directory containing the 12 Free*.ttf files from GNU FreeFont",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=project_root / "lib" / "Fonts" / "FontTurkishExtensions.generated.h",
    )
    parser.add_argument(
        "--preview",
        type=Path,
        help="Optional PNG contact sheet for visual verification",
    )
    arguments = parser.parse_args()

    font_directory = project_root / "lib" / "Fonts"
    verify_freefont_rasterization(font_directory, arguments.freefont_dir)
    generated: dict[str, list[Glyph] | None] = {}
    for font_name in FONT_NAMES:
        if font_name == "Apple6x10":
            # Apple6x10 already contains its own hand-drawn Turkish range.
            generated[font_name] = None
            continue
        configuration = freefont_configuration(font_name)
        if configuration is not None:
            font = ttf_font(arguments.freefont_dir, font_name)
            generated[font_name] = [
                rasterize_ttf_glyph(font, character)
                for character in TURKISH_CHARACTERS
            ]
            continue
        header = font_directory / BITMAP_FONT_HEADERS[font_name]
        source_glyphs = parse_gfx_font(header, font_name)
        generated[font_name] = decorate_bitmap_font(source_glyphs)

    validate_extensions(generated)
    output = generate_header(generated)
    arguments.output.write_text(output, encoding="utf-8", newline="\n")
    if arguments.preview:
        render_preview(generated, arguments.preview)
    bitmap_bytes = sum(
        sum(len(pack_glyph(glyph)) for glyph in glyphs)
        for glyphs in generated.values()
        if glyphs is not None
    )
    print(
        f"Generated {sum(glyphs is not None for glyphs in generated.values())} font extensions, "
        f"{bitmap_bytes} bitmap bytes -> {arguments.output}"
    )


if __name__ == "__main__":
    main()
