#pragma once

#include <Adafruit_GFX.h>
#include <stddef.h>
#include <stdint.h>

// Adafruit GFX font Apple6x10 size: 6x10
// Extended with Turkish glyphs and degree sign in the private 0x7F-0x8B range, and German and
// Spanish letter codes in 0x8C-0x9B. UTF-8 text must be converted with Apple6x10EncodeUtf8()
// before it is drawn.

const uint8_t Apple6x10Bitmaps[] PROGMEM = {
  0xFA,	// exclam
  0xB6,0x80,	// quotedbl
  0x52,0xBE,0xAF,0xA9,0x40,	// numbersign
  0x23,0xA8,0xE2,0xB8,0x80,	// dollar
  0x4D,0x54,0x45,0x56,0x40,	// percent
  0x45,0x28,0x8A,0xC9,0xA0,	// ampersand
  0xE0,	// quotesingle
  0x2A,0x48,0x88,	// parenleft
  0x88,0x92,0xA0,	// parenright
  0x8A,0xBE,0xA8,0x80,	// asterisk
  0x21,0x3E,0x42,0x00,	// plus
  0x6A,0x00,	// comma
  0xF8,	// hyphen
  0x5D,0x00,	// period
  0x08,0x44,0x44,0x42,0x00,	// slash
  0x22,0xA3,0x18,0xA8,0x80,	// zero
  0x23,0x28,0x42,0x13,0xE0,	// one
  0x74,0x42,0x64,0x43,0xE0,	// two
  0xF8,0x44,0x60,0xC5,0xC0,	// three
  0x11,0x95,0x2F,0x88,0x40,	// four
  0xFC,0x2D,0x90,0xC5,0xC0,	// five
  0x32,0x21,0x6C,0xC5,0xC0,	// six
  0xF8,0x44,0x22,0x21,0x00,	// seven
  0x74,0x62,0xE8,0xC5,0xC0,	// eight
  0x74,0x66,0xD0,0x89,0x80,	// nine
  0x5D,0x05,0xD0,	// colon
  0x5D,0x06,0xA0,	// semicolon
  0x12,0x48,0x42,0x10,	// less
  0xF8,0x3E,	// equal
  0x84,0x21,0x24,0x80,	// greater
  0x74,0x44,0x42,0x00,0x80,	// question
  0x74,0x67,0x5B,0x41,0xC0,	// at
  0x22,0xA3,0x1F,0xC6,0x20,	// A
  0xF2,0x52,0xE4,0xA7,0xC0,	// B
  0x74,0x61,0x08,0x45,0xC0,	// C
  0xF2,0x52,0x94,0xA7,0xC0,	// D
  0xFC,0x21,0xE8,0x43,0xE0,	// E
  0xFC,0x21,0xE8,0x42,0x00,	// F
  0x74,0x61,0x09,0xC5,0xC0,	// G
  0x8C,0x63,0xF8,0xC6,0x20,	// H
  0xE9,0x24,0xB8,	// I
  0x38,0x84,0x21,0x49,0x80,	// J
  0x8C,0xA9,0x8A,0x4A,0x20,	// K
  0x84,0x21,0x08,0x43,0xE0,	// L
  0x8C,0x77,0x58,0xC6,0x20,	// M
  0x8C,0x73,0x59,0xC6,0x20,	// N
  0x74,0x63,0x18,0xC5,0xC0,	// O
  0xF4,0x63,0xE8,0x42,0x00,	// P
  0x74,0x63,0x18,0xD5,0xC1,	// Q
  0xF4,0x63,0xEA,0x4A,0x20,	// R
  0x74,0x60,0xE0,0xC5,0xC0,	// S
  0xF9,0x08,0x42,0x10,0x80,	// T
  0x8C,0x63,0x18,0xC5,0xC0,	// U
  0x8C,0x62,0xA5,0x28,0x80,	// V
  0x8C,0x63,0x5A,0xEE,0x20,	// W
  0x8C,0x54,0x45,0x46,0x20,	// X
  0x8C,0x54,0x42,0x10,0x80,	// Y
  0xF8,0x44,0x44,0x43,0xE0,	// Z
  0xF2,0x49,0x38,	// bracketleft
  0x84,0x10,0x41,0x04,0x20,	// backslash
  0xE4,0x92,0x78,	// bracketright
  0x22,0xA2,	// asciicircum
  0xF8,	// underscore
  0x90,	// grave
  0x70,0x5F,0x17,0x80,	// a
  0x84,0x2D,0x98,0xE6,0xC0,	// b
  0x74,0x61,0x17,0x00,	// c
  0x08,0x5B,0x38,0xCD,0xA0,	// d
  0x74,0x7F,0x07,0x00,	// e
  0x32,0x51,0xE4,0x21,0x00,	// f
  0x7C,0x62,0xF0,0xC5,0xC0,	// g
  0x84,0x2D,0x98,0xC6,0x20,	// h
  0x43,0x24,0xB8,	// i
  0x10,0x31,0x11,0x99,0x60,	// j
  0x84,0x23,0x2E,0x4A,0x20,	// k
  0xC9,0x24,0xB8,	// l
  0xD5,0x6B,0x58,0x80,	// m
  0xB6,0x63,0x18,0x80,	// n
  0x74,0x63,0x17,0x00,	// o
  0xB6,0x63,0x9B,0x42,0x00,	// p
  0x6C,0xE3,0x36,0x84,0x20,	// q
  0xB6,0x61,0x08,0x00,	// r
  0x74,0x1C,0x1F,0x00,	// s
  0x42,0x3C,0x84,0x24,0xC0,	// t
  0x8C,0x63,0x36,0x80,	// u
  0x8C,0x54,0xA2,0x00,	// v
  0x8C,0x6B,0x55,0x00,	// w
  0x8A,0x88,0xA8,0x80,	// x
  0x8C,0x66,0xD0,0xC5,0xC0,	// y
  0xF8,0x88,0x8F,0x80,	// z
  0x34,0x2C,0x24,0x30,	// braceleft
  0xFE,	// bar
  0xC2,0x43,0x42,0xC0,	// braceright
  0x4D,0x64,	// asciitilde
  0x74,0x61,0x08,0x45,0xC4,	// Ccedilla
  0x53,0xA3,0x08,0x4E,0x2E,	// Gbreve
  0x5D,0x24,0x97,	// Idotaccent
  0x53,0xA3,0x18,0xC6,0x2E,	// Odieresis
  0x74,0x60,0xE0,0xC5,0xC4,	// Scedilla
  0x54,0x63,0x18,0xC6,0x2E,	// Udieresis
  0x74,0x61,0x17,0x10,	// ccedilla
  0x50,0x1F,0x18,0xBC,0x31,0x70,	// gbreve
  0xC9,0x2E,	// dotlessi
  0x50,0x1D,0x18,0xC5,0xC0,	// odieresis
  0x74,0x1C,0x1F,0x10,	// scedilla
  0x50,0x23,0x18,0xCD,0xA0,	// udieresis
  0x69,0x96	// degree
};

const GFXglyph Apple6x10Glyphs[] PROGMEM = {
  { 0, 0, 0, 6, 0, 0 },	// space
  { 0, 1, 7, 6, 2, -8 },	// exclam
  { 1, 3, 3, 6, 1, -8 },	// quotedbl
  { 3, 5, 7, 6, 0, -8 },	// numbersign
  { 8, 5, 7, 6, 0, -8 },	// dollar
  { 13, 5, 7, 6, 0, -8 },	// percent
  { 18, 5, 7, 6, 0, -8 },	// ampersand
  { 23, 1, 3, 6, 2, -8 },	// quotesingle
  { 24, 3, 7, 6, 1, -8 },	// parenleft
  { 27, 3, 7, 6, 1, -8 },	// parenright
  { 30, 5, 5, 6, 0, -7 },	// asterisk
  { 34, 5, 5, 6, 0, -7 },	// plus
  { 38, 3, 3, 6, 1, -3 },	// comma
  { 40, 5, 1, 6, 0, -5 },	// hyphen
  { 41, 3, 3, 6, 1, -3 },	// period
  { 43, 5, 7, 6, 0, -8 },	// slash
  { 48, 5, 7, 6, 0, -8 },	// zero
  { 53, 5, 7, 6, 0, -8 },	// one
  { 58, 5, 7, 6, 0, -8 },	// two
  { 63, 5, 7, 6, 0, -8 },	// three
  { 68, 5, 7, 6, 0, -8 },	// four
  { 73, 5, 7, 6, 0, -8 },	// five
  { 78, 5, 7, 6, 0, -8 },	// six
  { 83, 5, 7, 6, 0, -8 },	// seven
  { 88, 5, 7, 6, 0, -8 },	// eight
  { 93, 5, 7, 6, 0, -8 },	// nine
  { 98, 3, 7, 6, 1, -7 },	// colon
  { 101, 3, 7, 6, 1, -7 },	// semicolon
  { 104, 4, 7, 6, 1, -8 },	// less
  { 108, 5, 3, 6, 0, -6 },	// equal
  { 110, 4, 7, 6, 1, -8 },	// greater
  { 114, 5, 7, 6, 0, -8 },	// question
  { 119, 5, 7, 6, 0, -8 },	// at
  { 124, 5, 7, 6, 0, -8 },	// A
  { 129, 5, 7, 6, 0, -8 },	// B
  { 134, 5, 7, 6, 0, -8 },	// C
  { 139, 5, 7, 6, 0, -8 },	// D
  { 144, 5, 7, 6, 0, -8 },	// E
  { 149, 5, 7, 6, 0, -8 },	// F
  { 154, 5, 7, 6, 0, -8 },	// G
  { 159, 5, 7, 6, 0, -8 },	// H
  { 164, 3, 7, 6, 1, -8 },	// I
  { 167, 5, 7, 6, 0, -8 },	// J
  { 172, 5, 7, 6, 0, -8 },	// K
  { 177, 5, 7, 6, 0, -8 },	// L
  { 182, 5, 7, 6, 0, -8 },	// M
  { 187, 5, 7, 6, 0, -8 },	// N
  { 192, 5, 7, 6, 0, -8 },	// O
  { 197, 5, 7, 6, 0, -8 },	// P
  { 202, 5, 8, 6, 0, -8 },	// Q
  { 207, 5, 7, 6, 0, -8 },	// R
  { 212, 5, 7, 6, 0, -8 },	// S
  { 217, 5, 7, 6, 0, -8 },	// T
  { 222, 5, 7, 6, 0, -8 },	// U
  { 227, 5, 7, 6, 0, -8 },	// V
  { 232, 5, 7, 6, 0, -8 },	// W
  { 237, 5, 7, 6, 0, -8 },	// X
  { 242, 5, 7, 6, 0, -8 },	// Y
  { 247, 5, 7, 6, 0, -8 },	// Z
  { 252, 3, 7, 6, 1, -8 },	// bracketleft
  { 255, 5, 7, 6, 0, -8 },	// backslash
  { 260, 3, 7, 6, 1, -8 },	// bracketright
  { 263, 5, 3, 6, 0, -8 },	// asciicircum
  { 265, 5, 1, 6, 0, -1 },	// underscore
  { 266, 2, 2, 6, 2, -9 },	// grave
  { 267, 5, 5, 6, 0, -6 },	// a
  { 271, 5, 7, 6, 0, -8 },	// b
  { 276, 5, 5, 6, 0, -6 },	// c
  { 280, 5, 7, 6, 0, -8 },	// d
  { 285, 5, 5, 6, 0, -6 },	// e
  { 289, 5, 7, 6, 0, -8 },	// f
  { 294, 5, 7, 6, 0, -6 },	// g
  { 299, 5, 7, 6, 0, -8 },	// h
  { 304, 3, 7, 6, 1, -8 },	// i
  { 307, 4, 9, 6, 1, -8 },	// j
  { 312, 5, 7, 6, 0, -8 },	// k
  { 317, 3, 7, 6, 1, -8 },	// l
  { 320, 5, 5, 6, 0, -6 },	// m
  { 324, 5, 5, 6, 0, -6 },	// n
  { 328, 5, 5, 6, 0, -6 },	// o
  { 332, 5, 7, 6, 0, -6 },	// p
  { 337, 5, 7, 6, 0, -6 },	// q
  { 342, 5, 5, 6, 0, -6 },	// r
  { 346, 5, 5, 6, 0, -6 },	// s
  { 350, 5, 7, 6, 0, -8 },	// t
  { 355, 5, 5, 6, 0, -6 },	// u
  { 359, 5, 5, 6, 0, -6 },	// v
  { 363, 5, 5, 6, 0, -6 },	// w
  { 367, 5, 5, 6, 0, -6 },	// x
  { 371, 5, 7, 6, 0, -6 },	// y
  { 376, 5, 5, 6, 0, -6 },	// z
  { 380, 4, 7, 6, 1, -8 },	// braceleft
  { 384, 1, 7, 6, 2, -8 },	// bar
  { 385, 4, 7, 6, 1, -8 },	// braceright
  { 389, 5, 3, 6, 0, -8 },	// asciitilde
  { 391, 5, 8, 6, 0, -8 },	// Ccedilla
  { 396, 5, 8, 6, 0, -9 },	// Gbreve
  { 401, 3, 8, 6, 1, -9 },	// Idotaccent
  { 404, 5, 8, 6, 0, -9 },	// Odieresis
  { 409, 5, 8, 6, 0, -8 },	// Scedilla
  { 414, 5, 8, 6, 0, -9 },	// Udieresis
  { 419, 5, 6, 6, 0, -6 },	// ccedilla
  { 423, 5, 9, 6, 0, -8 },	// gbreve
  { 429, 3, 5, 6, 1, -6 },	// dotlessi
  { 431, 5, 7, 6, 0, -8 },	// odieresis
  { 436, 5, 6, 6, 0, -6 },	// scedilla
  { 440, 5, 7, 6, 0, -8 },	// udieresis
  { 445, 4, 4, 6, 1, -8 }	// degree, top aligned with the digits
};

const GFXfont Apple6x10 PROGMEM = {
 (uint8_t *)Apple6x10Bitmaps,
 (GFXglyph *)Apple6x10Glyphs,
 32, 139, 10};

constexpr uint8_t Apple6x10TurkishCcedilla = 0x7F;
constexpr uint8_t Apple6x10TurkishGbreve = 0x80;
constexpr uint8_t Apple6x10TurkishIdot = 0x81;
constexpr uint8_t Apple6x10TurkishOdieresis = 0x82;
constexpr uint8_t Apple6x10TurkishScedilla = 0x83;
constexpr uint8_t Apple6x10TurkishUdieresis = 0x84;
constexpr uint8_t Apple6x10TurkishSmallCcedilla = 0x85;
constexpr uint8_t Apple6x10TurkishSmallGbreve = 0x86;
constexpr uint8_t Apple6x10TurkishDotlessI = 0x87;
constexpr uint8_t Apple6x10TurkishSmallOdieresis = 0x88;
constexpr uint8_t Apple6x10TurkishSmallScedilla = 0x89;
constexpr uint8_t Apple6x10TurkishSmallUdieresis = 0x8A;
constexpr uint8_t Apple6x10Degree = 0x8B;

// German and Spanish letters. No font contains them natively; FontCatalog
// synthesizes them from each font's own base letter and accent geometry.
constexpr uint8_t Apple6x10Adieresis = 0x8C;
constexpr uint8_t Apple6x10SmallAdieresis = 0x8D;
constexpr uint8_t Apple6x10Aacute = 0x8E;
constexpr uint8_t Apple6x10SmallAacute = 0x8F;
constexpr uint8_t Apple6x10Eacute = 0x90;
constexpr uint8_t Apple6x10SmallEacute = 0x91;
constexpr uint8_t Apple6x10Iacute = 0x92;
constexpr uint8_t Apple6x10SmallIacute = 0x93;
constexpr uint8_t Apple6x10Oacute = 0x94;
constexpr uint8_t Apple6x10SmallOacute = 0x95;
constexpr uint8_t Apple6x10Uacute = 0x96;
constexpr uint8_t Apple6x10SmallUacute = 0x97;
constexpr uint8_t Apple6x10Ntilde = 0x98;
constexpr uint8_t Apple6x10SmallNtilde = 0x99;
constexpr uint8_t Apple6x10InvertedExclamation = 0x9A;
constexpr uint8_t Apple6x10InvertedQuestion = 0x9B;
constexpr uint8_t Apple6x10LastExtendedGlyph = Apple6x10InvertedQuestion;

inline uint8_t Apple6x10GlyphForCodePoint(uint32_t codePoint)
{
  switch (codePoint) {
    case 0x00C4: return Apple6x10Adieresis;
    case 0x00E4: return Apple6x10SmallAdieresis;
    case 0x00C1: return Apple6x10Aacute;
    case 0x00E1: return Apple6x10SmallAacute;
    case 0x00C9: return Apple6x10Eacute;
    case 0x00E9: return Apple6x10SmallEacute;
    case 0x00CD: return Apple6x10Iacute;
    case 0x00ED: return Apple6x10SmallIacute;
    case 0x00D3: return Apple6x10Oacute;
    case 0x00F3: return Apple6x10SmallOacute;
    case 0x00DA: return Apple6x10Uacute;
    case 0x00FA: return Apple6x10SmallUacute;
    case 0x00D1: return Apple6x10Ntilde;
    case 0x00F1: return Apple6x10SmallNtilde;
    case 0x00A1: return Apple6x10InvertedExclamation;
    case 0x00BF: return Apple6x10InvertedQuestion;
    case 0x00C7: return Apple6x10TurkishCcedilla;
    case 0x011E: return Apple6x10TurkishGbreve;
    case 0x0130: return Apple6x10TurkishIdot;
    case 0x00D6: return Apple6x10TurkishOdieresis;
    case 0x015E: return Apple6x10TurkishScedilla;
    case 0x00DC: return Apple6x10TurkishUdieresis;
    case 0x00E7: return Apple6x10TurkishSmallCcedilla;
    case 0x011F: return Apple6x10TurkishSmallGbreve;
    case 0x0131: return Apple6x10TurkishDotlessI;
    case 0x00F6: return Apple6x10TurkishSmallOdieresis;
    case 0x015F: return Apple6x10TurkishSmallScedilla;
    case 0x00FC: return Apple6x10TurkishSmallUdieresis;
    case 0x00B0: return Apple6x10Degree;
    default: return '?';
  }
}

inline size_t Apple6x10EncodeUtf8(
  const char *input,
  char *output,
  size_t outputSize)
{
  if (output == nullptr || outputSize == 0) {
    return 0;
  }
  if (input == nullptr) {
    output[0] = '\0';
    return 0;
  }

  size_t readIndex = 0;
  size_t writeIndex = 0;
  while (input[readIndex] != '\0' && writeIndex + 1 < outputSize) {
    const uint8_t first = static_cast<uint8_t>(input[readIndex]);
    if (first < 0x80) {
      output[writeIndex++] = static_cast<char>(first);
      ++readIndex;
      continue;
    }

    uint32_t codePoint = 0;
    uint8_t sequenceLength = 0;
    if (first >= 0xC2 && first <= 0xDF) {
      sequenceLength = 2;
      codePoint = first & 0x1F;
    } else if (first >= 0xE0 && first <= 0xEF) {
      sequenceLength = 3;
      codePoint = first & 0x0F;
    } else if (first >= 0xF0 && first <= 0xF4) {
      sequenceLength = 4;
      codePoint = first & 0x07;
    } else {
      output[writeIndex++] = '?';
      ++readIndex;
      continue;
    }

    bool validSequence = true;
    for (uint8_t offset = 1; offset < sequenceLength; ++offset) {
      const uint8_t continuation =
        static_cast<uint8_t>(input[readIndex + offset]);
      if (continuation == 0 || (continuation & 0xC0) != 0x80) {
        validSequence = false;
        break;
      }
      codePoint = (codePoint << 6) | (continuation & 0x3F);
    }

    if (!validSequence) {
      output[writeIndex++] = '?';
      ++readIndex;
      continue;
    }

    if (codePoint == 0x2103 && writeIndex + 2 < outputSize) {
      // U+2103 CELSIUS SIGN is rendered as the shared degree glyph plus C.
      output[writeIndex++] = static_cast<char>(Apple6x10Degree);
      output[writeIndex++] = 'C';
    } else if (codePoint == 0x00DF && writeIndex + 2 < outputSize) {
      // German sharp s has no compact bitmap form; "ss" is its official
      // replacement spelling.
      output[writeIndex++] = 's';
      output[writeIndex++] = 's';
    } else {
      output[writeIndex++] =
        static_cast<char>(Apple6x10GlyphForCodePoint(codePoint));
    }
    readIndex += sequenceLength;
  }

  output[writeIndex] = '\0';
  return writeIndex;
}

