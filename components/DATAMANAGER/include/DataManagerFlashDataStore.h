#pragma once 

#include "Common.h"
#include<map>
#include "DataManagerPrivate.h"

class DataManagerFlashDataBucket {
    uint8_t *_buffer = nullptr;
    
public:
    DataManagerFlashDataBucket();
    ~DataManagerFlashDataBucket();
    FlashDataBlockHeader* GetHeader();
    uint GetSize();
    uint GetPayloadSize();
    uint8_t* GetPayload();
    bool static Intersects(const FlashDataBlockHeader& pHeader, time_t pFrom, time_t pTo);
};

class DataManagerFlashDataStore {
    const esp_partition_t* _partition;
    int _latestValid = -1;
    int _oldestValid = -1;
    std::map<uint,FlashDataBlockHeader> _blocks; 
    void ScanBuckets();
   
public:
    DataManagerFlashDataStore();
    ~DataManagerFlashDataStore();

    int CalcPrevious(int pCurrent);
    int CalcNext(int pCurrent);
    int GetOldest();
    int GetLatest();

    uint GetBucketCount() const;
    bool LoadBucket(uint pIndex, DataManagerFlashDataBucket* pBucket);
    bool GetBucketHeader(uint pIndex, FlashDataBlockHeader* pHeader);
    uint GetBucketSize() const;
    void SaveBucket(DataManagerFlashDataBucket* pBucket);
};