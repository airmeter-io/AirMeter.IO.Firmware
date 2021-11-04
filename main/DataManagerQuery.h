#pragma once

#include "Common.h"
#include "DataManager.h"
#include "DataManagerFlashDataStore.h"

class DataManagerQuery {
    DataManagerFlashDataStore& _flashStore;
    IDataManagerTemporaryStore& _store;
    time_t _from;
    time_t _to;
    DataManagerFlashDataBucket _bucket;
    uint _bufferIndex  = 0;
    int _bucketIndex = -1;
    bool _loadedTemp = false;
    DataEntry _dataEntry;
    uint ProcessBuffer(DataEntry *pEntries, uint pMaxEntries);

public:
    DataManagerQuery(DataManagerFlashDataStore& pFlashStore, IDataManagerTemporaryStore& pStore, time_t pFrom, time_t pTo);
    ~DataManagerQuery();
    uint ReadEntries(DataEntry *pEntries, uint pMaxEntries);
};