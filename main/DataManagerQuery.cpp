#include "DataManagerQuery.h"
#include "DataManagerPrivate.h"



DataManagerQuery::DataManagerQuery(DataManagerFlashDataStore& pFlashStore,IDataManagerTemporaryStore& pStore, time_t pFrom, time_t pTo) : 
    _flashStore(pFlashStore),_store(pStore), _from(pFrom), _to(pTo) {
    _bucket.GetHeader()->DataLength = 0;
    _bucketIndex = pFlashStore.GetOldest();
}

DataManagerQuery::~DataManagerQuery() {
}

uint DataManagerQuery::ProcessBuffer(DataEntry *pEntries, uint pMaxEntries) {
    auto count = 0;
 //  printf("Processing buffer %u\n", _bucket.GetHeader()->DataLength);
    auto buffer = _bucket.GetPayload();
    while(_bufferIndex < _bucket.GetHeader()->DataLength && count<pMaxEntries) {
    //    printf("Bufindex %u\n", _bufferIndex);
        auto* full = (DataPointFull *)(buffer+_bufferIndex);
        auto* compact = (DataPointCompact *)(buffer+_bufferIndex);
        auto* veryCompact = (DataPointVeryCompact *)(buffer+_bufferIndex);
        switch((DataPointType)full->Type) {
            case DataPointType::Full :
                if(_bufferIndex+sizeof(DataPointFull)  > _bucket.GetHeader()->DataLength) {                
                    _bufferIndex = _bucket.GetHeader()->DataLength;
                }
                _dataEntry.TimeStamp = full->TimeStamp;
                _dataEntry.Temp = full->Temp;
                _dataEntry.CO2 = full->CO2;
                _dataEntry.Humidity = full->Humidity;
                _dataEntry.Pressure = full->Pressure;            
                _bufferIndex+=sizeof(DataPointFull);
                break;
            case DataPointType::Compact :
                if(_bufferIndex+sizeof(DataPointCompact)  > _bucket.GetHeader()->DataLength) {
                    _bufferIndex =  _bucket.GetHeader()->DataLength;
                }
                _dataEntry.TimeStamp+=compact->TimeDelta;
                _dataEntry.Temp+=compact->TempDelta;
                _dataEntry.CO2+=compact->CO2Delta;
                _dataEntry.Humidity+=compact->HumidityDelta;
                _dataEntry.Pressure+=compact->PressureDelta;
                _bufferIndex+=sizeof(DataPointCompact);
                break;
            case DataPointType::VeryCompact :
                if(_bufferIndex+sizeof(DataPointVeryCompact)  > _bucket.GetHeader()->DataLength) {
                    _bufferIndex =  _bucket.GetHeader()->DataLength;
                }
                _dataEntry.TimeStamp+=veryCompact->TimeDelta;
                _dataEntry.Temp+=veryCompact->TempDelta;
                _dataEntry.CO2+=veryCompact->CO2Delta;
                _dataEntry.Humidity+=veryCompact->HumidityDelta;
                _dataEntry.Pressure+=veryCompact->PressureDelta;
                _bufferIndex+=sizeof(DataPointVeryCompact);
                break;
        }
     
        if(_from>_dataEntry.TimeStamp && _to>_dataEntry.TimeStamp ) {
     //       printf("Skipping Happend in past timestamp %u\n", (uint)_dataEntry.TimeStamp);
            continue;
        }
        if(_from<_dataEntry.TimeStamp && _to<_dataEntry.TimeStamp ){
        //    printf("Skipping Happend in future timestamp %u\n", (uint)_dataEntry.TimeStamp);
            break;            
        }
            
        pEntries[count] = _dataEntry;
        count++;        
    }

    if(_bufferIndex>=_bucket.GetHeader()->DataLength) {
        _bufferIndex = _bucket.GetHeader()->DataLength;
    }
    printf("Reading %d items\n",count);
    return count;
}

uint DataManagerQuery::ReadEntries(DataEntry *pEntries, uint pMaxEntries) {
    if(_bufferIndex < _bucket.GetHeader()->DataLength) {
      //  printf("Got some buffer processing it\n");
        auto result = ProcessBuffer(pEntries, pMaxEntries);
        if(result) return result;

       // printf("Not enough entries continuing\n");
    }
  //  printf("bucket index %d\n", _bucketIndex);
    if(_bucketIndex!=-1) {
        _flashStore.LoadBucket(_bucketIndex, &_bucket);
    //    printf("Got flash block processing more of it\n");      
        _bufferIndex = 0;
        _bucketIndex = _bucketIndex==_flashStore.GetLatest()? -1 : _flashStore.CalcNext(_bucketIndex);
        if(_bucket.GetHeader()->DataLength>0)
            return ProcessBuffer(pEntries, pMaxEntries);
    //    printf("Not enough buffer continuing\n");
        
        while(_bucketIndex!=-1) {
       //     printf("Loading next bucket\n");
            _flashStore.LoadBucket(_bucketIndex, &_bucket);
            _bucketIndex = _bucketIndex==_flashStore.GetLatest()? -1 : _flashStore.CalcNext(_bucketIndex);
            auto header =  _bucket.GetHeader();
            if((_from<header->BlockStartTime && _to<header->BlockStartTime)) {
             //   printf("Bucket in past skipping\n");
                continue;
            } else {
                if(_from>header->BlockEndTime && _to>header->BlockEndTime ) {
               //     printf("Bucket in future returning 0 entries\n");
                    return 0;
                } 
                if(_bucket.GetHeader()->DataLength>0)
                    return ProcessBuffer(pEntries, pMaxEntries);
             //   printf("Not enough buffer continuing\n");
                break;
            }
        }
    
    } 

    
    if(!_loadedTemp){
        auto size = _store.GetUsed();
    //   printf("Getting temp size %u, %u\n", size,_bucket.GetHeader()->DataLength);
        if(size> _bucket.GetPayloadSize()) {
    //     printf("Bucket not big enough to take temporary storage data\n");
            return 0;
        }
        if(size>0) {
            _bucket.GetHeader()->DataLength = _store.Read(_bucket.GetPayload(), 0, size);
            _bufferIndex = 0;
            _loadedTemp = true;
            if(_bucket.GetHeader()->DataLength>0)
                return ProcessBuffer(pEntries, pMaxEntries);
        }
        
    }
    return 0;
}