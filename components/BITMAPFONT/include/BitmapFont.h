#pragma once
#include "FontFileStructure.h"
#include <tinyutf8/tinyutf8.h>
#include "EPDBackBuffer.h"
#include <string>


typedef struct {
   int32_t width;
   int32_t height;
} Dimensions;

typedef struct {
   int32_t x;
   int32_t y;
} Position;

typedef struct {
   Position pos;
   Dimensions size;
} Rectangle;


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


class IFontBltTarget {
public:
    virtual ~IFontBltTarget()  {}
    virtual void BltMonoBitmap(uint8_t* pData, Dimensions pBitmapSize, Position pDrawAt, Rectangle pClipRectangle, EPDColor pColor) = 0;
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
        IFontBltTarget& pTarget,
        Rectangle pDrawAt, 
        EPDColor pColor, 
        DrawTextJustify pJustify, 
        DrawTextVerticalAlign pVerticalAlign,
        uint pLineSpacing);
};