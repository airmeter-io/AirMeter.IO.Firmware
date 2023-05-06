#pragma once

#include "Common.h"
#include<vector>
#include<map>
#include "ValueController.h"
#include "DataManagerStoreBucket.h"

typedef struct {
    uint32_t Offset;
    uint32_t Index;
    time_t BlockStartTime;
    time_t BlockEndTime;
    uint32_t DataLength;
} DataStoreBucketInfo;

class DataManagerStore {
    const esp_partition_t* _partition;
    uint32_t _currentBucket;
    DataStoreBucketInfo* _buckets; 
    uint32_t _numBuckets;
    DataManagerStoreBucket* _bucket = nullptr;
    std::vector<ValueSource*> _values;
    void ScanBuckets();
    void OpenBucket(uint32_t pIndex);
public:
    DataManagerStore();
    ~DataManagerStore();

    void EraseAll();

    void WriteRecord();
    void GetBucketsForRange(time_t pFrom, time_t pTo, std::vector<DataStoreBucketInfo*>& pResult);
    inline const esp_partition_t * GetPartition() { return _partition; }

};