#pragma once

#include "Common.h"
#include<vector>
#include<map>
#include "ValueController.h"

enum FlashDataBlockRecordType {
    FullRecord = 0, 
    Delta3BitRecord = 1, 
    Delta5BitRecord = 2, 
    Delta7BitRecord = 3, 
    Delta11BitRecord = 4, 
    IndexRecord = 6,
    FreeRecord = 7
};


typedef struct {
    uint32_t Magic;
    uint32_t Index;
    uint16_t ValueCount;
    time_t BlockStartTime;
    time_t BlockEndTime;
    time_t OpenTime;
    time_t CloseTime;
    uint32_t DataLength;
    uint32_t NumReadings;
} __attribute__((packed)) DataStoreBucketHeader;

typedef struct  {
    uint Type:2; // DataPointType::Full
    int TimeDelta:30;
} __attribute__((packed))  FlashDataBlockIndexRecordHeader;

enum DataManagerStoreBucketState {
    UnusedBucket,
    OpenBucket,
    CompletedBucket,
    InvalidBucket
};

class DataManagerStoreBucket {
    DataStoreBucketHeader _header;
    const esp_partition_t* _partition;
    uint _payloadOffset = 0;
    time_t _lastReading;
    bool _firstRecord = true;
    uint32_t _numReadings;
    DataManagerStoreBucketState _state;
    size_t _offset;
    size_t _size;
    std::vector<ValueSource*> _values;
    int16_t* _lastValues  = nullptr;
    void ReadHeader();
    void CloseBucket();
    void WriteHeader();
    bool WriteRecord(uint8_t* pData, uint32_t pDatalength);
    bool WriteDeltaRecord();
    bool WriteFullRecord();
    void GetCurrentValues(int16_t* pValues);

public:
    DataManagerStoreBucket(const esp_partition_t*  pPartition, size_t pOffset, size_t pSize);
    ~DataManagerStoreBucket();
    DataStoreBucketHeader* GetHeader();
    DataManagerStoreBucketState GetState();
    bool Open(const std::vector<ValueSource*>& pValues);
    void Close();
    void Erase();
    bool WriteRecord();

};