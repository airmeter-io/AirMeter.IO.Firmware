#pragma once

#include "Common.h"

class BitWriter {
    uint8_t* _buffer;
    size_t _bufferSize;
    uint32_t _bitIndex = 0;
    uint32_t _byteIndex = 0;
    uint32_t _localBitIndex = 0;
    uint32_t _localBitsRemaining = 8;
public:
    BitWriter(uint8_t* pBuffer, size_t pBufferSize) : _buffer(pBuffer), _bufferSize(pBufferSize) {
        _buffer[0] = 0;
    }
    inline void WriteUInt(uint32_t pUnsigned, uint16_t pBitLength) {
        auto toEncode = ((1 << pBitLength) - 1) & pUnsigned;
        auto bitsRemaining = pBitLength;
        while (bitsRemaining > 0) {
            auto bitsToEncode = bitsRemaining < _localBitsRemaining ? bitsRemaining : _localBitsRemaining;
            uint8_t current = (uint8_t)toEncode;
            current <<= _localBitIndex;
            _buffer[_byteIndex] |= current;
            toEncode >>= bitsToEncode;
            _bitIndex += bitsToEncode;
            _localBitsRemaining -= bitsToEncode;
            bitsRemaining -= bitsToEncode;
            if (_localBitsRemaining)
                _localBitIndex += bitsToEncode;
            else {
                _localBitIndex = 0;
                _localBitsRemaining = 8;
                _byteIndex++;
                _buffer[_byteIndex] = 0;
            }
        }
    }
    inline void WriteInt(int32_t pSigned, uint16_t pBitLength) {
        uint32_t squashed = pSigned < 0 ? 1 : 0;
        squashed <<= pBitLength;
        squashed += abs(pSigned);
        WriteUInt(squashed, pBitLength + 1);
    }

    inline uint32_t GetByteLength() {
        return _byteIndex+1;
    }
};