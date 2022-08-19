#include "EPDBackBuffer.h"

EPDBackBuffer::EPDBackBuffer(uint pWidth, uint pHeight) : _width(pWidth), _height(pHeight) {
    _bufferLength = (pWidth * pHeight)/8;
    _buffer = (uint8_t*)malloc(_bufferLength);
}

EPDBackBuffer::~EPDBackBuffer() {
    free(_buffer);
}

void EPDBackBuffer::Fill(DrawColor pColor) {
    switch(pColor) {
        case DrawColor::Black : 
            memset(_buffer, 0xFF, _bufferLength);
            break;
        case DrawColor::White :
            memset(_buffer, 0x00, _bufferLength);
            break;
    }
}

uint8_t* EPDBackBuffer::GetBuffer() const {
    return _buffer;
}

uint EPDBackBuffer::GetBufferLength() const {
    return _bufferLength;
}

uint EPDBackBuffer::GetWidth() const {
    return _width;
}

uint EPDBackBuffer::GetHeight() const {
    return _height;
}