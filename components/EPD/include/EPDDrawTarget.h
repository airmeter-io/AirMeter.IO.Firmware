#pragma once

#include "Common.h"
#include "EPDBackBuffer.h"
#include "BitmapFont.h"

// Note: GDEW0213I5F is our test display that will be the default initializing this class
class EPDDrawTarget : public IFontBltTarget
{
    EPDBackBuffer& _buffer;
    uint16_t _rotation;
public:
    EPDDrawTarget(EPDBackBuffer& pBuffer);
    ~EPDDrawTarget();

    void BltMonoBitmap(uint8_t* pData, Dimensions pBitmapSize, Position pDrawAt, Rectangle pClipRectangle, uint16_t pColor) override;
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void setRotation(int16_t pRotation);

    inline EPDBackBuffer& GetBackBuffer() {
        return _buffer;
    }
};