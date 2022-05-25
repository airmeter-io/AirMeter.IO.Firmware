#include "DataManagerRamTemporaryStore.h"
#include "DataManagerPrivate.h"

//#define BUFFER_SIZE 50
#define BUFFER_SIZE (4096-sizeof(FlashDataBlockHeader))

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
RTC_DATA_ATTR uint8_t Temporary[BUFFER_SIZE];
#else
uint8_t Temporary[BUFFER_SIZE];
#endif

void DataManagerRamTemporaryStore::Flush(time_t pBlockStartTime)  {
    _blockStartTime = pBlockStartTime;
    _blockEndTime = pBlockStartTime;
    _writeIndex = 0;
}

bool DataManagerRamTemporaryStore::CanAddData(uint pAmount) {
    return _writeIndex+pAmount < BUFFER_SIZE;
}

uint DataManagerRamTemporaryStore::GetUsed() {
    return _writeIndex;
}

uint DataManagerRamTemporaryStore::Read(uint8_t *pDest, uint pOffset, uint pLength) {
    if(pOffset+pLength>BUFFER_SIZE) {
        printf("Adjusting read length from %u, %u\n", pLength, BUFFER_SIZE - pOffset);
        pLength = BUFFER_SIZE - pOffset;
    } 
    memcpy(pDest, _buffer+pOffset, pLength);
    return pLength;
}

uint DataManagerRamTemporaryStore::Write(time_t pTimeStamp, uint8_t *pSource, uint pLength) {
    if(_writeIndex+pLength>BUFFER_SIZE) {
        printf("Adjusting write length from %u, %u\n", pLength, BUFFER_SIZE - _writeIndex);
        pLength = BUFFER_SIZE - _writeIndex;
    }
    memcpy(_buffer+_writeIndex,pSource, pLength);
    _writeIndex+=pLength;
    if(pTimeStamp>_blockEndTime) _blockEndTime = pTimeStamp;
    return pLength;
}

time_t DataManagerRamTemporaryStore::GetBlockStartTime() {
    return _blockStartTime;
}

time_t DataManagerRamTemporaryStore::GetBlockEndTime() {
    return _blockEndTime;
}

DataManagerRamTemporaryStore::DataManagerRamTemporaryStore() {
    _buffer = Temporary;
}

DataManagerRamTemporaryStore::~DataManagerRamTemporaryStore() {
}
