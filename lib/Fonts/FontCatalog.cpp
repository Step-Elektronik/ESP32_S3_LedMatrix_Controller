#include "FontCatalog.h"

#include <cstring>
#include <limits.h>

#include <FontGfx_apple4x6.h>
#include <FontGfx_apple5x7.h>
#include <FontGfx_apple6x10.h>
#include <FontGfx_apple8x13.h>
#include <FontGfx_gohufont6x11.h>
#include <FontGfx_gohufont6x11b.h>
#include <FontGfx_tom_thumb.h>
#include <FreeMono9pt7b.h>
#include <FreeMono12pt7b.h>
#include <FreeMono18pt7b.h>
#include <FreeMono24pt7b.h>
#include <FreeMonoBold9pt7b.h>
#include <FreeMonoBold12pt7b.h>
#include <FreeMonoBold18pt7b.h>
#include <FreeMonoBold24pt7b.h>
#include <FreeMonoBoldOblique9pt7b.h>
#include <FreeMonoBoldOblique12pt7b.h>
#include <FreeMonoBoldOblique18pt7b.h>
#include <FreeMonoBoldOblique24pt7b.h>
#include <FreeMonoOblique9pt7b.h>
#include <FreeMonoOblique12pt7b.h>
#include <FreeMonoOblique18pt7b.h>
#include <FreeMonoOblique24pt7b.h>
#include <FreeSans9pt7b.h>
#include <FreeSans12pt7b.h>
#include <FreeSans18pt7b.h>
#include <FreeSans24pt7b.h>
#include <FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <FreeSansBold18pt7b.h>
#include <FreeSansBold24pt7b.h>
#include <FreeSansBoldOblique9pt7b.h>
#include <FreeSansBoldOblique12pt7b.h>
#include <FreeSansBoldOblique18pt7b.h>
#include <FreeSansBoldOblique24pt7b.h>
#include <FreeSansOblique9pt7b.h>
#include <FreeSansOblique12pt7b.h>
#include <FreeSansOblique18pt7b.h>
#include <FreeSansOblique24pt7b.h>
#include <FreeSerif9pt7b.h>
#include <FreeSerif12pt7b.h>
#include <FreeSerif18pt7b.h>
#include <FreeSerif24pt7b.h>
#include <FreeSerifBold9pt7b.h>
#include <FreeSerifBold12pt7b.h>
#include <FreeSerifBold18pt7b.h>
#include <FreeSerifBold24pt7b.h>
#include <FreeSerifBoldItalic9pt7b.h>
#include <FreeSerifBoldItalic12pt7b.h>
#include <FreeSerifBoldItalic18pt7b.h>
#include <FreeSerifBoldItalic24pt7b.h>
#include <FreeSerifItalic9pt7b.h>
#include <FreeSerifItalic12pt7b.h>
#include <FreeSerifItalic18pt7b.h>
#include <FreeSerifItalic24pt7b.h>
#include <Org_01.h>
#include <Picopixel.h>
#include <Tiny3x3a2pt7b.h>
#include <TomThumb.h>

namespace {

struct FontEntry {
  const char *name;
  const GFXfont *font;
};

constexpr FontEntry fontCatalog[] = {
  {"Apple4x6", &Apple4x6},
  {"Apple5x7", &Apple5x7},
  {"Apple6x10", &Apple6x10},
  {"Apple8x13", &Apple8x13},
  {"GohuFont6x11", &GohuFont6x11},
  {"GohuFont6x11b", &GohuFont6x11b},
  {"Fixed", &Fixed},
  {"FreeMono9pt7b", &FreeMono9pt7b},
  {"FreeMono12pt7b", &FreeMono12pt7b},
  {"FreeMono18pt7b", &FreeMono18pt7b},
  {"FreeMono24pt7b", &FreeMono24pt7b},
  {"FreeMonoBold9pt7b", &FreeMonoBold9pt7b},
  {"FreeMonoBold12pt7b", &FreeMonoBold12pt7b},
  {"FreeMonoBold18pt7b", &FreeMonoBold18pt7b},
  {"FreeMonoBold24pt7b", &FreeMonoBold24pt7b},
  {"FreeMonoBoldOblique9pt7b", &FreeMonoBoldOblique9pt7b},
  {"FreeMonoBoldOblique12pt7b", &FreeMonoBoldOblique12pt7b},
  {"FreeMonoBoldOblique18pt7b", &FreeMonoBoldOblique18pt7b},
  {"FreeMonoBoldOblique24pt7b", &FreeMonoBoldOblique24pt7b},
  {"FreeMonoOblique9pt7b", &FreeMonoOblique9pt7b},
  {"FreeMonoOblique12pt7b", &FreeMonoOblique12pt7b},
  {"FreeMonoOblique18pt7b", &FreeMonoOblique18pt7b},
  {"FreeMonoOblique24pt7b", &FreeMonoOblique24pt7b},
  {"FreeSans9pt7b", &FreeSans9pt7b},
  {"FreeSans12pt7b", &FreeSans12pt7b},
  {"FreeSans18pt7b", &FreeSans18pt7b},
  {"FreeSans24pt7b", &FreeSans24pt7b},
  {"FreeSansBold9pt7b", &FreeSansBold9pt7b},
  {"FreeSansBold12pt7b", &FreeSansBold12pt7b},
  {"FreeSansBold18pt7b", &FreeSansBold18pt7b},
  {"FreeSansBold24pt7b", &FreeSansBold24pt7b},
  {"FreeSansBoldOblique9pt7b", &FreeSansBoldOblique9pt7b},
  {"FreeSansBoldOblique12pt7b", &FreeSansBoldOblique12pt7b},
  {"FreeSansBoldOblique18pt7b", &FreeSansBoldOblique18pt7b},
  {"FreeSansBoldOblique24pt7b", &FreeSansBoldOblique24pt7b},
  {"FreeSansOblique9pt7b", &FreeSansOblique9pt7b},
  {"FreeSansOblique12pt7b", &FreeSansOblique12pt7b},
  {"FreeSansOblique18pt7b", &FreeSansOblique18pt7b},
  {"FreeSansOblique24pt7b", &FreeSansOblique24pt7b},
  {"FreeSerif9pt7b", &FreeSerif9pt7b},
  {"FreeSerif12pt7b", &FreeSerif12pt7b},
  {"FreeSerif18pt7b", &FreeSerif18pt7b},
  {"FreeSerif24pt7b", &FreeSerif24pt7b},
  {"FreeSerifBold9pt7b", &FreeSerifBold9pt7b},
  {"FreeSerifBold12pt7b", &FreeSerifBold12pt7b},
  {"FreeSerifBold18pt7b", &FreeSerifBold18pt7b},
  {"FreeSerifBold24pt7b", &FreeSerifBold24pt7b},
  {"FreeSerifBoldItalic9pt7b", &FreeSerifBoldItalic9pt7b},
  {"FreeSerifBoldItalic12pt7b", &FreeSerifBoldItalic12pt7b},
  {"FreeSerifBoldItalic18pt7b", &FreeSerifBoldItalic18pt7b},
  {"FreeSerifBoldItalic24pt7b", &FreeSerifBoldItalic24pt7b},
  {"FreeSerifItalic9pt7b", &FreeSerifItalic9pt7b},
  {"FreeSerifItalic12pt7b", &FreeSerifItalic12pt7b},
  {"FreeSerifItalic18pt7b", &FreeSerifItalic18pt7b},
  {"FreeSerifItalic24pt7b", &FreeSerifItalic24pt7b},
  {"Org_01", &Org_01},
  {"Picopixel", &Picopixel},
  {"Tiny3x3a2pt7b", &Tiny3x3a2pt7b},
  {"TomThumb", &TomThumb}
};

static_assert(
  sizeof(fontCatalog) / sizeof(fontCatalog[0]) == FontModel::fontCount,
  "FontModel::fontCount must match the compiled font catalog");

struct TurkishExtensionGlyph {
  uint16_t bitmapOffset;
  uint8_t width;
  uint8_t height;
  uint8_t xAdvance;
  int8_t xOffset;
  int8_t yOffset;
};

struct TurkishExtensionFont {
  const uint8_t *bitmaps;
  const TurkishExtensionGlyph *glyphs;
};

#include "FontTurkishExtensions.generated.h"

static_assert(
  FontCatalog::lastGlyphCode == Apple6x10LastExtendedGlyph,
  "FontCatalog::lastGlyphCode must cover every encoded glyph");

enum class GlyphDecoration : uint8_t {
  cedilla,
  breve,
  dot,
  diaeresis,
  dotless,
  degree,
  acute,
  tilde,
  inverted
};

bool syntheticGlyphSpec(
  uint8_t character,
  uint8_t &baseCharacter,
  GlyphDecoration &decoration,
  bool &removeDot)
{
  removeDot = false;
  switch (character) {
    case Apple6x10Adieresis:
      baseCharacter = 'A'; decoration = GlyphDecoration::diaeresis; return true;
    case Apple6x10SmallAdieresis:
      baseCharacter = 'a'; decoration = GlyphDecoration::diaeresis; return true;
    case Apple6x10Aacute:
      baseCharacter = 'A'; decoration = GlyphDecoration::acute; return true;
    case Apple6x10SmallAacute:
      baseCharacter = 'a'; decoration = GlyphDecoration::acute; return true;
    case Apple6x10Eacute:
      baseCharacter = 'E'; decoration = GlyphDecoration::acute; return true;
    case Apple6x10SmallEacute:
      baseCharacter = 'e'; decoration = GlyphDecoration::acute; return true;
    case Apple6x10Iacute:
      baseCharacter = 'I'; decoration = GlyphDecoration::acute; return true;
    case Apple6x10SmallIacute:
      baseCharacter = 'i'; decoration = GlyphDecoration::acute; removeDot = true; return true;
    case Apple6x10Oacute:
      baseCharacter = 'O'; decoration = GlyphDecoration::acute; return true;
    case Apple6x10SmallOacute:
      baseCharacter = 'o'; decoration = GlyphDecoration::acute; return true;
    case Apple6x10Uacute:
      baseCharacter = 'U'; decoration = GlyphDecoration::acute; return true;
    case Apple6x10SmallUacute:
      baseCharacter = 'u'; decoration = GlyphDecoration::acute; return true;
    case Apple6x10Ntilde:
      baseCharacter = 'N'; decoration = GlyphDecoration::tilde; return true;
    case Apple6x10SmallNtilde:
      baseCharacter = 'n'; decoration = GlyphDecoration::tilde; return true;
    case Apple6x10InvertedExclamation:
      baseCharacter = '!'; decoration = GlyphDecoration::inverted; return true;
    case Apple6x10InvertedQuestion:
      baseCharacter = '?'; decoration = GlyphDecoration::inverted; return true;
    case Apple6x10TurkishCcedilla:
      baseCharacter = 'C'; decoration = GlyphDecoration::cedilla; return true;
    case Apple6x10TurkishGbreve:
      baseCharacter = 'G'; decoration = GlyphDecoration::breve; return true;
    case Apple6x10TurkishIdot:
      baseCharacter = 'I'; decoration = GlyphDecoration::dot; return true;
    case Apple6x10TurkishOdieresis:
      baseCharacter = 'O'; decoration = GlyphDecoration::diaeresis; return true;
    case Apple6x10TurkishScedilla:
      baseCharacter = 'S'; decoration = GlyphDecoration::cedilla; return true;
    case Apple6x10TurkishUdieresis:
      baseCharacter = 'U'; decoration = GlyphDecoration::diaeresis; return true;
    case Apple6x10TurkishSmallCcedilla:
      baseCharacter = 'c'; decoration = GlyphDecoration::cedilla; return true;
    case Apple6x10TurkishSmallGbreve:
      baseCharacter = 'g'; decoration = GlyphDecoration::breve; return true;
    case Apple6x10TurkishDotlessI:
      baseCharacter = 'i'; decoration = GlyphDecoration::dotless; removeDot = true; return true;
    case Apple6x10TurkishSmallOdieresis:
      baseCharacter = 'o'; decoration = GlyphDecoration::diaeresis; return true;
    case Apple6x10TurkishSmallScedilla:
      baseCharacter = 's'; decoration = GlyphDecoration::cedilla; return true;
    case Apple6x10TurkishSmallUdieresis:
      baseCharacter = 'u'; decoration = GlyphDecoration::diaeresis; return true;
    case Apple6x10Degree:
      baseCharacter = 'O'; decoration = GlyphDecoration::degree; return true;
    default:
      return false;
  }
}

bool nativeGlyph(
  const GFXfont *selectedFont,
  uint8_t character,
  FontCatalog::GlyphView &result)
{
  const uint8_t first = pgm_read_byte(&selectedFont->first);
  const uint8_t last = pgm_read_byte(&selectedFont->last);
  if (character < first || character > last) {
    return false;
  }

  const GFXglyph *glyphs = reinterpret_cast<const GFXglyph *>(
    pgm_read_ptr(&selectedFont->glyph));
  const uint8_t *bitmaps = reinterpret_cast<const uint8_t *>(
    pgm_read_ptr(&selectedFont->bitmap));
  const GFXglyph &glyph = glyphs[character - first];
  const uint16_t bitmapOffset = pgm_read_word(&glyph.bitmapOffset);
  result.width = pgm_read_byte(&glyph.width);
  result.height = pgm_read_byte(&glyph.height);
  result.xAdvance = pgm_read_byte(&glyph.xAdvance);
  result.xOffset = static_cast<int8_t>(pgm_read_byte(&glyph.xOffset));
  result.yOffset = static_cast<int8_t>(pgm_read_byte(&glyph.yOffset));
  result.bitmapSize =
    (static_cast<size_t>(result.width) * result.height + 7U) / 8U;
  result.bitmap = &bitmaps[bitmapOffset];
  result.bitmapInProgramMemory = true;
  return true;
}

bool fontSpecificTurkishGlyph(
  uint8_t fontId,
  uint8_t character,
  FontCatalog::GlyphView &result)
{
  if (fontId >= FontModel::fontCount
      || character < turkishExtensionFirst
      || character >= turkishExtensionFirst + turkishExtensionGlyphCount) {
    return false;
  }

  const TurkishExtensionFont &extension = turkishExtensionFonts[fontId];
  const uint8_t *bitmaps = reinterpret_cast<const uint8_t *>(
    pgm_read_ptr(&extension.bitmaps));
  const TurkishExtensionGlyph *glyphs =
    reinterpret_cast<const TurkishExtensionGlyph *>(
      pgm_read_ptr(&extension.glyphs));
  if (bitmaps == nullptr || glyphs == nullptr) {
    return false;
  }

  const TurkishExtensionGlyph &glyph =
    glyphs[character - turkishExtensionFirst];
  const uint16_t bitmapOffset = pgm_read_word(&glyph.bitmapOffset);
  result.width = pgm_read_byte(&glyph.width);
  result.height = pgm_read_byte(&glyph.height);
  result.xAdvance = pgm_read_byte(&glyph.xAdvance);
  result.xOffset = static_cast<int8_t>(pgm_read_byte(&glyph.xOffset));
  result.yOffset = static_cast<int8_t>(pgm_read_byte(&glyph.yOffset));
  result.bitmapSize =
    (static_cast<size_t>(result.width) * result.height + 7U) / 8U;
  result.bitmap = &bitmaps[bitmapOffset];
  result.bitmapInProgramMemory = true;
  return true;
}

bool bitmapPixel(
  const FontCatalog::GlyphView &glyph,
  uint8_t x,
  uint8_t y)
{
  const size_t bitIndex = static_cast<size_t>(y) * glyph.width + x;
  const uint8_t *byteAddress = &glyph.bitmap[bitIndex >> 3U];
  const uint8_t byteValue = glyph.bitmapInProgramMemory
    ? pgm_read_byte(byteAddress)
    : *byteAddress;
  return (byteValue & (0x80U >> (bitIndex & 7U))) != 0;
}

void setBitmapPixel(
  uint8_t *bitmap,
  uint8_t width,
  uint8_t height,
  int16_t x,
  int16_t y)
{
  if (x < 0 || y < 0 || x >= width || y >= height) {
    return;
  }
  const size_t bitIndex = static_cast<size_t>(y) * width + x;
  bitmap[bitIndex >> 3U] |= static_cast<uint8_t>(0x80U >> (bitIndex & 7U));
}

bool rowHasPixel(const FontCatalog::GlyphView &glyph, uint8_t row)
{
  for (uint8_t x = 0; x < glyph.width; ++x) {
    if (bitmapPixel(glyph, x, row)) {
      return true;
    }
  }
  return false;
}

bool hasPixelAfterRow(const FontCatalog::GlyphView &glyph, uint8_t row)
{
  for (uint8_t y = row + 1U; y < glyph.height; ++y) {
    if (rowHasPixel(glyph, y)) {
      return true;
    }
  }
  return false;
}

bool synthesizeGlyph(
  const GFXfont *selectedFont,
  uint8_t character,
  uint8_t *generatedBitmap,
  size_t generatedBitmapCapacity,
  FontCatalog::GlyphView &result)
{
  uint8_t baseCharacter = 0;
  GlyphDecoration decoration = GlyphDecoration::dot;
  bool removeDot = false;
  if (!syntheticGlyphSpec(character, baseCharacter, decoration, removeDot)
      || generatedBitmap == nullptr) {
    return false;
  }

  FontCatalog::GlyphView base;
  if (!nativeGlyph(selectedFont, baseCharacter, base)) {
    return false;
  }

  const int16_t baseLeft = base.xOffset;
  const int16_t baseTop = base.yOffset;
  const int16_t baseRight = baseLeft + base.width - 1;
  const int16_t baseBottom = baseTop + base.height - 1;
  const int16_t centerX = baseLeft + static_cast<int16_t>(base.width - 1U) / 2;
  const uint8_t stroke = base.height >= 32 ? 3 : (base.height >= 16 ? 2 : 1);
  const uint8_t gap = 1;

  if (decoration == GlyphDecoration::inverted) {
    // Spanish inverted punctuation: the base glyph rotated by 180 degrees in
    // its own bounding box.
    result.width = base.width;
    result.height = base.height;
    result.xAdvance = base.xAdvance;
    result.xOffset = base.xOffset;
    result.yOffset = base.yOffset;
    result.bitmapSize =
      (static_cast<size_t>(base.width) * base.height + 7U) / 8U;
    if (result.bitmapSize > generatedBitmapCapacity) {
      return false;
    }
    std::memset(generatedBitmap, 0, result.bitmapSize);
    for (uint8_t y = 0; y < base.height; ++y) {
      for (uint8_t x = 0; x < base.width; ++x) {
        if (bitmapPixel(base, x, y)) {
          setBitmapPixel(generatedBitmap, base.width, base.height,
            base.width - 1 - x, base.height - 1 - y);
        }
      }
    }
    result.bitmap = generatedBitmap;
    result.bitmapInProgramMemory = false;
    return true;
  }

  // Rows of the base glyph that belong to the dot of 'i' are dropped for the
  // dotless i and for i-acute, whose accent replaces the dot. Accents are
  // placed relative to the first remaining row, not the removed dot.
  int16_t dotlessSeparator = -1;
  uint8_t firstContentRow = 0;
  if (removeDot) {
    bool topPixelsSeen = false;
    for (uint8_t row = 0; row + 1U < base.height; ++row) {
      if (rowHasPixel(base, row)) {
        topPixelsSeen = true;
      } else if (topPixelsSeen && hasPixelAfterRow(base, row)) {
        dotlessSeparator = row;
        break;
      }
    }
    if (dotlessSeparator < 0 && base.height >= 3) {
      dotlessSeparator = stroke - 1U;
    }
    firstContentRow = static_cast<uint8_t>(dotlessSeparator + 1);
    while (firstContentRow + 1U < base.height && !rowHasPixel(base, firstContentRow)) {
      ++firstContentRow;
    }
  }
  const int16_t contentTop = baseTop + firstContentRow;

  int16_t decorationLeft = baseLeft;
  int16_t decorationTop = contentTop;
  int16_t decorationRight = baseRight;
  int16_t decorationBottom = baseBottom;
  uint8_t decorationWidth = 0;
  uint8_t decorationHeight = 0;

  if (decoration == GlyphDecoration::dot) {
    decorationWidth = stroke;
    decorationHeight = stroke;
    decorationLeft = centerX - static_cast<int16_t>(decorationWidth - 1U) / 2;
    decorationRight = decorationLeft + decorationWidth - 1;
    decorationBottom = contentTop - gap - 1;
    decorationTop = decorationBottom - decorationHeight + 1;
  } else if (decoration == GlyphDecoration::diaeresis) {
    decorationWidth = stroke;
    decorationHeight = stroke;
    const int16_t separation = base.width >= 8 ? base.width / 3 : stroke + 1;
    const int16_t leftCenter = centerX - (separation + 1) / 2;
    const int16_t rightCenter = centerX + separation / 2;
    decorationLeft = leftCenter - static_cast<int16_t>(decorationWidth - 1U) / 2;
    decorationRight = rightCenter + decorationWidth / 2;
    decorationBottom = contentTop - gap - 1;
    decorationTop = decorationBottom - decorationHeight + 1;
  } else if (decoration == GlyphDecoration::breve) {
    decorationWidth = base.width < 3 ? 3 : static_cast<uint8_t>(
      (static_cast<uint16_t>(base.width) * 2U + 2U) / 3U);
    if ((decorationWidth & 1U) == 0) {
      ++decorationWidth;
    }
    decorationHeight = stroke * 2U;
    decorationLeft = centerX - static_cast<int16_t>(decorationWidth - 1U) / 2;
    decorationRight = decorationLeft + decorationWidth - 1;
    decorationBottom = contentTop - gap - 1;
    decorationTop = decorationBottom - decorationHeight + 1;
  } else if (decoration == GlyphDecoration::acute) {
    // A diagonal stroke rising to the right.
    decorationHeight = stroke * 2U;
    decorationWidth = decorationHeight + stroke - 1U;
    decorationLeft = centerX - static_cast<int16_t>(decorationWidth - 1U) / 2;
    decorationRight = decorationLeft + decorationWidth - 1;
    decorationBottom = contentTop - gap - 1;
    decorationTop = decorationBottom - decorationHeight + 1;
  } else if (decoration == GlyphDecoration::tilde) {
    const uint8_t scaledWidth = static_cast<uint8_t>(
      (static_cast<uint16_t>(base.width) * 2U + 2U) / 3U);
    decorationWidth = scaledWidth < 4 ? 4 : scaledWidth;
    decorationHeight = stroke * 2U;
    decorationLeft = centerX - static_cast<int16_t>(decorationWidth - 1U) / 2;
    decorationRight = decorationLeft + decorationWidth - 1;
    decorationBottom = contentTop - gap - 1;
    decorationTop = decorationBottom - decorationHeight + 1;
  } else if (decoration == GlyphDecoration::cedilla) {
    decorationWidth = stroke * 3U + 1U;
    if (decorationWidth > base.width && base.width >= 2) {
      decorationWidth = base.width;
    }
    decorationHeight = stroke * 3U;
    decorationLeft = centerX - static_cast<int16_t>(decorationWidth - 1U) / 2;
    decorationRight = decorationLeft + decorationWidth - 1;
    decorationTop = baseBottom + 1;
    decorationBottom = decorationTop + decorationHeight - 1;
  } else if (decoration == GlyphDecoration::degree) {
    uint8_t diameter = static_cast<uint8_t>((base.height + 3U) / 4U);
    if (diameter < 3) diameter = 3;
    if (diameter > 12) diameter = 12;
    result.width = diameter;
    result.height = diameter;
    result.xAdvance = base.xAdvance / 2U;
    if (result.xAdvance < diameter + 1U) result.xAdvance = diameter + 1U;
    result.xOffset = base.xOffset;
    result.yOffset = base.yOffset;
    result.bitmapSize =
      (static_cast<size_t>(diameter) * diameter + 7U) / 8U;
    if (result.bitmapSize > generatedBitmapCapacity) {
      return false;
    }
    std::memset(generatedBitmap, 0, result.bitmapSize);
    const uint8_t ringStroke = stroke > diameter / 3U ? 1 : stroke;
    for (uint8_t layer = 0; layer < ringStroke; ++layer) {
      const int16_t minimum = layer;
      const int16_t maximum = diameter - 1U - layer;
      for (int16_t coordinate = minimum + 1; coordinate < maximum; ++coordinate) {
        setBitmapPixel(generatedBitmap, diameter, diameter, coordinate, minimum);
        setBitmapPixel(generatedBitmap, diameter, diameter, coordinate, maximum);
        setBitmapPixel(generatedBitmap, diameter, diameter, minimum, coordinate);
        setBitmapPixel(generatedBitmap, diameter, diameter, maximum, coordinate);
      }
    }
    result.bitmap = generatedBitmap;
    result.bitmapInProgramMemory = false;
    return true;
  }

  const bool hasDecoration = decoration != GlyphDecoration::dotless;
  const int16_t glyphLeft = hasDecoration && decorationLeft < baseLeft
    ? decorationLeft : baseLeft;
  const int16_t glyphTop = hasDecoration && decorationTop < contentTop
    ? decorationTop : contentTop;
  const int16_t glyphRight = hasDecoration && decorationRight > baseRight
    ? decorationRight : baseRight;
  const int16_t glyphBottom = hasDecoration && decorationBottom > baseBottom
    ? decorationBottom : baseBottom;
  const int16_t glyphWidth = glyphRight - glyphLeft + 1;
  const int16_t glyphHeight = glyphBottom - glyphTop + 1;
  if (glyphWidth <= 0 || glyphHeight <= 0
      || glyphWidth > UINT8_MAX || glyphHeight > UINT8_MAX
      || glyphLeft < INT8_MIN || glyphLeft > INT8_MAX
      || glyphTop < INT8_MIN || glyphTop > INT8_MAX) {
    return false;
  }

  result.width = static_cast<uint8_t>(glyphWidth);
  result.height = static_cast<uint8_t>(glyphHeight);
  result.xAdvance = base.xAdvance;
  result.xOffset = static_cast<int8_t>(glyphLeft);
  result.yOffset = static_cast<int8_t>(glyphTop);
  result.bitmapSize =
    (static_cast<size_t>(result.width) * result.height + 7U) / 8U;
  if (result.bitmapSize > generatedBitmapCapacity) {
    return false;
  }
  std::memset(generatedBitmap, 0, result.bitmapSize);

  for (uint8_t y = 0; y < base.height; ++y) {
    if (dotlessSeparator >= 0 && y <= dotlessSeparator) {
      continue;
    }
    for (uint8_t x = 0; x < base.width; ++x) {
      if (bitmapPixel(base, x, y)) {
        setBitmapPixel(
          generatedBitmap,
          result.width,
          result.height,
          baseLeft + x - glyphLeft,
          baseTop + y - glyphTop);
      }
    }
  }

  const auto setDecorationPixel = [&](int16_t x, int16_t y) {
    setBitmapPixel(generatedBitmap, result.width, result.height,
      decorationLeft + x - glyphLeft, decorationTop + y - glyphTop);
  };

  if (decoration == GlyphDecoration::dot) {
    for (uint8_t y = 0; y < decorationHeight; ++y) {
      for (uint8_t x = 0; x < decorationWidth; ++x) {
        setDecorationPixel(x, y);
      }
    }
  } else if (decoration == GlyphDecoration::diaeresis) {
    const int16_t rightDotOffset = decorationRight - decorationWidth + 1
      - decorationLeft;
    for (uint8_t y = 0; y < decorationHeight; ++y) {
      for (uint8_t x = 0; x < decorationWidth; ++x) {
        setDecorationPixel(x, y);
        setDecorationPixel(rightDotOffset + x, y);
      }
    }
  } else if (decoration == GlyphDecoration::breve) {
    for (uint8_t y = 0; y < decorationHeight; ++y) {
      for (uint8_t x = 0; x < decorationWidth; ++x) {
        const bool side = y < decorationHeight - stroke
          && (x < stroke || x >= decorationWidth - stroke);
        const bool bottom = y >= decorationHeight - stroke
          && x >= stroke && x < decorationWidth - stroke;
        if (side || bottom) {
          setDecorationPixel(x, y);
        }
      }
    }
  } else if (decoration == GlyphDecoration::acute) {
    for (uint8_t y = 0; y < decorationHeight; ++y) {
      const uint8_t start = static_cast<uint8_t>(
        (decorationHeight - 1U - y) * (decorationWidth - stroke)
        / (decorationHeight - 1U));
      for (uint8_t x = start; x < start + stroke; ++x) {
        setDecorationPixel(x, y);
      }
    }
  } else if (decoration == GlyphDecoration::tilde) {
    // Low-high-low-high wave over four equal segments.
    for (uint8_t x = 0; x < decorationWidth; ++x) {
      const uint8_t segment = static_cast<uint8_t>(x * 4U / decorationWidth);
      const uint8_t firstRow = (segment & 1U) != 0 ? 0 : stroke;
      for (uint8_t y = firstRow; y < firstRow + stroke; ++y) {
        setDecorationPixel(x, y);
      }
    }
  } else if (decoration == GlyphDecoration::cedilla) {
    const uint8_t center = decorationWidth / 2U;
    for (uint8_t y = 0; y < decorationHeight; ++y) {
      for (uint8_t x = 0; x < decorationWidth; ++x) {
        const bool stem = y < decorationHeight - stroke
          && x >= center && x < center + stroke;
        const bool hook = y >= decorationHeight - stroke && x <= center;
        if (stem || hook) {
          setDecorationPixel(x, y);
        }
      }
    }
  }

  result.bitmap = generatedBitmap;
  result.bitmapInProgramMemory = false;
  return true;
}

} // namespace

namespace FontCatalog {

bool isValid(uint8_t fontId)
{
  return fontId < FontModel::fontCount;
}

const char *name(uint8_t fontId)
{
  const uint8_t safeId = isValid(fontId)
    ? fontId
    : FontModel::defaultFontId;
  return fontCatalog[safeId].name;
}

const GFXfont *font(uint8_t fontId)
{
  const uint8_t safeId = isValid(fontId)
    ? fontId
    : FontModel::defaultFontId;
  return fontCatalog[safeId].font;
}

uint8_t yAdvance(uint8_t fontId)
{
  const GFXfont *selectedFont = font(fontId);
  return pgm_read_byte(&selectedFont->yAdvance);
}

bool glyph(
  uint8_t fontId,
  uint8_t character,
  uint8_t *generatedBitmap,
  size_t generatedBitmapCapacity,
  GlyphView &result)
{
  result = {};
  const uint8_t safeId = isValid(fontId)
    ? fontId
    : FontModel::defaultFontId;
  const GFXfont *selectedFont = fontCatalog[safeId].font;
  if (nativeGlyph(selectedFont, character, result)) {
    return true;
  }
  if (fontSpecificTurkishGlyph(safeId, character, result)) {
    return true;
  }
  return synthesizeGlyph(
    selectedFont,
    character,
    generatedBitmap,
    generatedBitmapCapacity,
    result);
}

void getTextBounds(
  uint8_t fontId,
  const char *text,
  int16_t &xOffset,
  int16_t &yOffset,
  uint16_t &width,
  uint16_t &height)
{
  xOffset = 0;
  yOffset = 0;
  width = 0;
  height = 0;
  if (text == nullptr) {
    return;
  }

  int16_t cursorX = 0;
  int16_t cursorY = 0;
  int16_t minimumX = INT16_MAX;
  int16_t minimumY = INT16_MAX;
  int16_t maximumX = -1;
  int16_t maximumY = -1;
  uint8_t generatedBitmap[generatedGlyphBufferSize] = {};
  for (const uint8_t *character =
         reinterpret_cast<const uint8_t *>(text);
       *character != 0;
       ++character) {
    if (*character == '\n') {
      cursorX = 0;
      cursorY += yAdvance(fontId);
      continue;
    }
    if (*character == '\r') {
      continue;
    }

    GlyphView selectedGlyph;
    if (!glyph(
          fontId,
          *character,
          generatedBitmap,
          sizeof(generatedBitmap),
          selectedGlyph)) {
      continue;
    }
    const int16_t glyphLeft = cursorX + selectedGlyph.xOffset;
    const int16_t glyphTop = cursorY + selectedGlyph.yOffset;
    const int16_t glyphRight = glyphLeft + selectedGlyph.width - 1;
    const int16_t glyphBottom = glyphTop + selectedGlyph.height - 1;
    if (glyphLeft < minimumX) minimumX = glyphLeft;
    if (glyphTop < minimumY) minimumY = glyphTop;
    if (glyphRight > maximumX) maximumX = glyphRight;
    if (glyphBottom > maximumY) maximumY = glyphBottom;
    cursorX += selectedGlyph.xAdvance;
  }

  if (maximumX >= minimumX) {
    xOffset = minimumX;
    width = static_cast<uint16_t>(maximumX - minimumX + 1);
  }
  if (maximumY >= minimumY) {
    yOffset = minimumY;
    height = static_cast<uint16_t>(maximumY - minimumY + 1);
  }
}

size_t encodeText(
  uint8_t fontId,
  const char *input,
  char *output,
  size_t outputSize)
{
  (void)fontId;
  return Apple6x10EncodeUtf8(input, output, outputSize);
}

} // namespace FontCatalog
