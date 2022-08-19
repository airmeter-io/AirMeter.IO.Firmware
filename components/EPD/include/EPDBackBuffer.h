#pragma once

#include "Common.h"
#include "DrawTarget.h"


class EPDBackBuffer {
    uint8_t *_buffer;
    uint _bufferLength;
    uint _width;
    uint _height;
public:
    EPDBackBuffer(uint pWidth, uint pHeight);
    ~EPDBackBuffer();

    void Fill(DrawColor pColor);
    uint8_t* GetBuffer() const;
    uint GetBufferLength() const;
    uint GetWidth() const;
    uint GetHeight() const;
};