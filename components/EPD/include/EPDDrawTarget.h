#pragma once

#include "Common.h"
#include "EPDBackBuffer.h"
#include "DrawTarget.h"

// Note: GDEW0213I5F is our test display that will be the default initializing this class
class EPDDrawTarget : public DrawTarget
{
    EPDBackBuffer& _buffer;
    uint16_t _rotation;

    inline bool IsXOutOfRange(int16_t pX) {
        return (pX < 0) || (pX >= _buffer.GetWidth());
    }

    inline bool IsYOutOfRange(int16_t pY) {
        return (pY < 0) || (pY >= _buffer.GetHeight());
    }

    void inline Swap(int16_t &a, int16_t &b) {
        auto c = a;
        a = b;
        b = c;
    }

    inline void AdjustCoOrdinatesForRotation(int16_t &pX, int16_t &pY) {
        switch (_rotation)
        {
            case 1:
                Swap(pX, pY);
                pX = _buffer.GetWidth() - pX - 1;
                break;
            case 2:
                pX = _buffer.GetWidth() - pX - 1;
                pY = _buffer.GetHeight() - pY - 1;
                break;
            case 3:
                Swap(pX, pY);
                pY = _buffer.GetHeight() - pY - 1;
            break;
        }
    }

    inline void DrawPixelUnchecked(int16_t pX, int16_t pY, DrawColor pColor) {
        // This is the trick to draw colors right. Genious Jean-Marc
        uint16_t i = pX / 8 + pY *  _buffer.GetWidth() / 8;
        if (pColor == DrawColor::White) {
            _buffer.GetBuffer()[i] = (_buffer.GetBuffer()[i] & (0xFF ^ (1 << (7 - pX % 8))));
            } else {
            _buffer.GetBuffer()[i] = (_buffer.GetBuffer()[i] | (1 << (7 - pX % 8)));
        }
    }
public:
    EPDDrawTarget(EPDBackBuffer& pBuffer);
    ~EPDDrawTarget();

    void BltMonoBitmap(uint8_t* pData, Dimensions pBitmapSize, Position pDrawAt, Rectangle pClipRectangle, DrawColor pColor) override;
    void DrawFilledRectangle(int16_t pX, int16_t pY, int16_t pW, int16_t pH, DrawColor pColor) override;

    void DrawPixel(int16_t x, int16_t y, DrawColor color) override;
    void SetRotation(int16_t pRotation) override;
    void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, DrawColor color) override;
    void DrawDottedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, DrawColor color) override;
    void Fill(DrawColor pColor) override;
    inline EPDBackBuffer& GetBackBuffer() {
        return _buffer;
    }
};