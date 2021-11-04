#pragma once

#include "Common.h"
#include "DataManagerPrivate.h"

class DataManagerFlashDataStore;
class DataManagerQuery;

class DataEntry {
public:
    time_t TimeStamp;
    int16_t Temp;
    uint16_t CO2; 
    uint16_t Humidity;  
    uint16_t Pressure;
};

class IDataManagerTemporaryStore {
public:
    virtual void Flush(time_t pBlockStartTime) = 0;
    virtual bool CanAddData(uint pAmount)  = 0;
    virtual uint GetUsed() = 0;
    virtual uint Read(uint8_t *pDest, uint pOffset, uint pLength) = 0;
    virtual uint Write(time_t pTimeStamp, uint8_t *pSource, uint pLength) = 0;
    virtual time_t GetBlockStartTime() = 0;
    virtual time_t GetBlockEndTime() = 0;
};



class DataManager {
    DataEntry *_last = nullptr;
    uint _written = 0;
    IDataManagerTemporaryStore* _store;
    DataManagerFlashDataStore* _flashStore;
    DataPointType GetRequiredBlockType(const DataEntry &pEntry);
    void WriteFullEntry(const DataEntry& pEntry);
    void WriteCompactEntry(const DataEntry& pEntry);
    void WriteVeryCompactEntry(const DataEntry& pEntry);
    void WriteToFlash();
public:
    DataManager(DataManagerFlashDataStore *pFlashStore, IDataManagerTemporaryStore* pStore);
    ~DataManager();

    void WriteEntry(const DataEntry& pEntry);
    DataManagerQuery* StartQuery(time_t pFrom, time_t pTo);
    void ClearAllData();
};