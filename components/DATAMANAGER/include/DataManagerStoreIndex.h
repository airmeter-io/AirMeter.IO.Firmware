#pragma once
#include "DataManagerStoreBucket.h"
#include "Common.h"

typedef struct {
    uint32_t Magic;
    uint32_t Index;
    time_t BlockStartTime;
    time_t BlockEndTime;
    uint32_t DataLength;
    uint32_t NumReadings;
    uint16_t ValueCount;
} __attribute__((packed)) DataStoreIndexHeader;

class DataManagerStoreIndex {
    const esp_partition_t* _partition;
    DataStoreIndexHeader _header;
    DataManagerStoreBucketState _state;
    size_t _offset;
    size_t _size;
    void ReadHeader();
    void WriteHeader();
public:
    DataManagerStoreIndex(const esp_partition_t*  pPartition, size_t pOffset, size_t pSize);
    ~DataManagerStoreIndex();
    void Open();
    void Erase();
};