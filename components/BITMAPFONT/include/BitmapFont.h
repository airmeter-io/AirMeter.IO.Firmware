#pragma once
#include "FontFileStructure.h"
#include <tinyutf8/tinyutf8.h>
#include <string>
#include "DrawTarget.h"


enum DrawTextJustify {
    Left,
    Right,
    Center
};


enum DrawTextVerticalAlign {
    Top,
    Middle,
    Bottom
};




typedef struct {
    FontCharacterGroupGlyph glyph;
    uint16_t offset;
} BitmapFontCharacterGroupGlyph;

typedef struct {
    FontCharacterGroupHeader groupHeader;
    uint8_t *data;
    BitmapFontCharacterGroupGlyph *glyphs;
} BitmapFontCharacterGroup;

class BitmapFont {
    FontHeader _header;
    int16_t _yOffset;
    BitmapFontCharacterGroup *_groups;
public:
    BitmapFont(const char *pPath);
    BitmapFont(const std::string& pPath);
    ~BitmapFont();
    bool GetGlyph(uint32_t pCodePoint, FontCharacterGroupGlyph** pGlpyh, uint8_t** pData);
    void MeasureUtf8(const tiny_utf8::string& pText, uint32_t pWrapTextAt, Dimensions& pMeasuredSized);
    void DrawUtf8(
        const tiny_utf8::string& pText, 
        DrawTarget& pTarget,
        Rectangle pDrawAt, 
        DrawColor pColor, 
        DrawTextJustify pJustify, 
        DrawTextVerticalAlign pVerticalAlign,
        uint pLineSpacing);
};