#pragma once
#include <cstdint>
#pragma pack(push, 1)
typedef struct { // Data stored PER GLYPH
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL corner
} FontCharacterGroupGlyph;

typedef struct { // Data stored for FONT AS A WHOLE:
	uint32_t codePointStart;
	uint8_t numCodePoints;
	uint32_t dataLength;
} FontCharacterGroupHeader;

typedef struct
{
	uint32_t magic;
	uint32_t numGroups;
	uint32_t size;
	uint8_t  yAdvance;    // Newline distance (y axis)
} FontHeader;
#pragma pack(pop)