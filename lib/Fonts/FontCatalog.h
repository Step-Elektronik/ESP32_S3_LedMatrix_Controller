#pragma once

#include <Adafruit_GFX.h>
#include <cstddef>
#include <stdint.h>

#include "../../include/FontModel.h"

namespace FontCatalog {

constexpr uint8_t firstGlyphCode = 0x20;
constexpr uint8_t lastGlyphCode = 0x8B;
constexpr size_t generatedGlyphBufferSize = 512;

struct GlyphView {
  const uint8_t *bitmap = nullptr;
  size_t bitmapSize = 0;
  uint8_t width = 0;
  uint8_t height = 0;
  uint8_t xAdvance = 0;
  int8_t xOffset = 0;
  int8_t yOffset = 0;
  bool bitmapInProgramMemory = true;
};

bool isValid(uint8_t fontId);
const char *name(uint8_t fontId);
const GFXfont *font(uint8_t fontId);
uint8_t yAdvance(uint8_t fontId);
bool glyph(
  uint8_t fontId,
  uint8_t character,
  uint8_t *generatedBitmap,
  size_t generatedBitmapCapacity,
  GlyphView &result);
void getTextBounds(
  uint8_t fontId,
  const char *text,
  int16_t &xOffset,
  int16_t &yOffset,
  uint16_t &width,
  uint16_t &height);

// Converts UTF-8 Turkish characters and the degree sign to the common private
// glyph range rendered by every font in this catalog.
size_t encodeText(
  uint8_t fontId,
  const char *input,
  char *output,
  size_t outputSize);

} // namespace FontCatalog
