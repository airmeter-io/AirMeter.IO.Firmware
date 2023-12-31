#pragma once

#include "Common.h"
#include<vector>
#include<map>
#include "ValueController.h"
#include "DataManagerStoreBucket.h"

typedef struct {
    DataManagerStoreBucketState State;
    uint32_t Offset;
    uint32_t Index;
    time_t BlockStartTime;
    time_t BlockEndTime;
    uint32_t DataLength;
    uint32_t NumReadings;
} DataStoreBucketInfo;

typedef struct {
    uint32_t currentIndex;
    uint32_t numReadings;
    time_t currentTime;
    size_t offset;
    size_t size;
    DataStoreBucketInfo info;
} DataManagerStoreCurrentBucketInfo;

class DataManagerStore {
    const esp_partition_t* _partition;
    uint32_t _currentBucket;
    DataStoreBucketInfo* _buckets; 
    uint32_t _numBuckets;
    DataManagerStoreBucket* _bucket = nullptr;
    std::vector<ValueSource*> _values;
   
    void OpenBucket(uint32_t pIndex);
    void FreeOldestBuckets();
public:
    DataManagerStore();
    ~DataManagerStore();

    void ScanBuckets();
    void EraseAll();
    void WriteRecord(time_t pTime);
    void GetBucketsForRange(time_t pFrom, time_t pTo, std::vector<DataStoreBucketInfo*>& pResult);

    inline const esp_partition_t * GetPartition() { return _partition; }
    inline uint32_t GetNumBuckets() { return _numBuckets;}
    inline const DataStoreBucketInfo& GetBucket(uint32_t pIndex) {  
        return (pIndex < _numBuckets) ? _buckets[pIndex] : *_buckets;}
    void GetCurrentBucketInfo(DataManagerStoreCurrentBucketInfo& pInfo);
};