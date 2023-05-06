#pragma once

#include "Common.h"

class BitReader {
    uint8_t* _buffer;
    size_t _bufferSize;
    uint32_t _bitIndex = 0;
    uint32_t _byteIndex = 0;
    uint32_t _localBitIndex = 0;
    uint32_t _localBitsRemaining = 8;

public:
    BitReader(uint8_t* pBuffer, size_t pBufferSize) : _buffer(pBuffer), _bufferSize(pBufferSize) {

    }

    inline uint32_t ReadUInt(uint16_t pBitLength) {
        auto bitsRemaining = pBitLength;
        uint32_t result = 0;
        auto resultIndex = 0;
        while (bitsRemaining > 0) {
            auto bitsToDecode = bitsRemaining < _localBitsRemaining ? bitsRemaining : _localBitsRemaining;
            uint32_t current = (uint32_t)(_buffer[_byteIndex] >> _localBitIndex) & ((1 << bitsToDecode) - 1);

            current <<= resultIndex;
            result |= current;
            resultIndex += bitsToDecode;

            _bitIndex += bitsToDecode;
            _localBitsRemaining -= bitsToDecode;
            bitsRemaining -= bitsToDecode;
            if (_localBitsRemaining)
                _localBitIndex += bitsToDecode;
            else {
                _localBitIndex = 0;
                _localBitsRemaining = 8;
                _byteIndex++;
            }
        }
        return result;
    }

    inline int32_t ReadInt(uint16_t pBitLength) {
        auto rawResult = ReadUInt(pBitLength + 1);
        auto signFlag = (int32_t)(1 << pBitLength);
        return (rawResult & (signFlag - 1)) * (rawResult & signFlag ? -1 : 1);
    }

    
    inline uint32_t GetByteLength() {
        return _byteIndex+1;
    }

    inline void ReadBytes(uint8_t* pData, uint32_t pLength) {
        memcpy(pData, _buffer+_byteIndex, pLength);
        _byteIndex+=pLength;
        _bitIndex = _byteIndex*8;
    }
    inline void Advance() {
        if(_bitIndex>0)
        {
            _bitIndex = 0;
            _byteIndex++;
        }
    }
};