#pragma once

#include "Common.h"
enum EPDColor {
    White,
    Black
};

class EPDBackBuffer {
    uint8_t *_buffer;
    uint _bufferLength;
    uint _width;
    uint _height;
public:
    EPDBackBuffer(uint pWidth, uint pHeight);
    ~EPDBackBuffer();

    void Fill(EPDColor pColor);
    uint8_t* GetBuffer() const;
    uint GetBufferLength() const;
    uint GetWidth() const;
    uint GetHeight() const;
};