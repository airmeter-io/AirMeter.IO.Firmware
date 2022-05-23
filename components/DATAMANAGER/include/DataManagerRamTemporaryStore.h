#pragma once

#include "Common.h"
#include "DataManager.h"

class DataManagerRamTemporaryStore : public IDataManagerTemporaryStore {
    uint8_t *_buffer;
    int _writeIndex = 0;
    time_t _blockStartTime;
    time_t _blockEndTime;
    
public:
    DataManagerRamTemporaryStore();
    ~DataManagerRamTemporaryStore();

    void Flush(time_t pBlockStartTime) override;
    bool CanAddData(uint pAmount) override;
    uint GetUsed() override;
    uint Read(uint8_t *pDest, uint pOffset, uint pLength) override;
    uint Write(time_t pTimeStamp, uint8_t *pSource, uint pLength) override;
    time_t GetBlockStartTime() override;
    time_t GetBlockEndTime() override;
};