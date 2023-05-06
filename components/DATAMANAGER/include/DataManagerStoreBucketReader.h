#pragma once

#include "DataManagerStoreBucket.h"
#include "BitReader.h"

class DataManagerStoreBucketReader {
    const esp_partition_t* _partition;
    size_t _offset;
    size_t _size;    
    uint8_t * _buffer;
    DataStoreBucketHeader* _header;
    std::vector<ValueSource*> _values;
    BitReader* _reader;
    uint32_t _headerSize;
    int16_t *_currentValues;
    time_t _currentTime;
public:
    DataManagerStoreBucketReader(const esp_partition_t*  pPartition, size_t pOffset, size_t pSize);
    ~DataManagerStoreBucketReader();

    const std::vector<ValueSource*>& GetValues() const; 

    const int16_t* GetCurrentValues() const;

    bool ReadNext();
    uint32_t GetCurrentBlockOffset();
    inline time_t GetCurrentTime() { return _currentTime; }
};