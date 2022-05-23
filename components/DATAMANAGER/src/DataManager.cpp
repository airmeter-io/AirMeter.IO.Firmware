#include "DataManager.h"
#include<math.h>
#include "DataManagerPrivate.h"
#include "DataManagerQuery.h"
#include "DataManagerFlashDataStore.h"



DataManager::DataManager(DataManagerFlashDataStore *pFlashStore, IDataManagerTemporaryStore* pStore) : _store(pStore), _flashStore(pFlashStore) {
   if(pthread_mutex_init (&_mutex, NULL) != 0){
     printf("Failed to initialize the spiffs mutex");
   }
}

DataManager::~DataManager() {
    if(_last!=nullptr) delete _last;
}

void DataManager::WriteToFlash() {
    DataManagerFlashDataBucket bucket;

    auto size = _store->GetUsed();
    auto header =bucket.GetHeader();
    header->BlockStartTime = _store->GetBlockStartTime();
    header->BlockEndTime = _store->GetBlockEndTime();
    
    header->DataLength = _store->Read(bucket.GetPayload(),0 , size);
    
    _flashStore->SaveBucket(&bucket);
}

void DataManager::ClearAllData() {
    printf("Truncating data\n");
    if(!truncate("/spiffs/datalog.dat",0)) 
        printf("Successfully truncated file\n");

}

DataPointType DataManager::GetRequiredBlockType(const DataEntry &pEntry) {
    auto co2Dif = abs((int)pEntry.CO2-(int)_last->CO2);
    auto tempDif = abs((int)pEntry.Temp-(int)_last->Temp);
    auto humidityDif = abs((int)pEntry.Humidity-(int)_last->Humidity);
    auto pressureDif = abs((int)pEntry.Pressure-(int)_last->Pressure);
    auto timeDif = abs((int64_t)pEntry.TimeStamp-(int64_t)_last->TimeStamp);

    if(co2Dif<=15 && tempDif<=15 && humidityDif<=31 && pressureDif<=15 && timeDif<=511)
        return DataPointType::VeryCompact;
    if(co2Dif<=127 && tempDif<=127 && humidityDif<=127 && pressureDif<=127 && timeDif<=16383)
        return DataPointType::Compact;    
    return DataPointType::Full;    
}

void DataManager::WriteFullEntry(const DataEntry& pEntry) {
    if(!_last || !_store->CanAddData(sizeof(DataPointFull))) 
    {
        printf("Flushing\n");
        if(_last) 
            WriteToFlash();
        
        _store->Flush(pEntry.TimeStamp);
    }

    DataPointFull entry = { 
        .Type = DataPointType::Full,  
        .Temp = pEntry.Temp,
        .CO2 = pEntry.CO2,
        .Humidity = pEntry.Humidity,
        .Pressure = pEntry.Pressure,
        .TimeStamp = (uint32_t)pEntry.TimeStamp
    };

    _store->Write(pEntry.TimeStamp,(uint8_t *)&entry, sizeof(entry));
}

void DataManager::WriteCompactEntry(const DataEntry& pEntry) {
    if(!_store->CanAddData(sizeof(DataPointCompact))) 
    {
        printf("Flushing\n");
        if(_last) 
            WriteToFlash();
        
        _store->Flush(pEntry.TimeStamp);
        WriteFullEntry(pEntry);
    }

    DataPointCompact entry = { 
        .Type = DataPointType::Compact,  
        .TimeDelta = (uint)(pEntry.TimeStamp-_last->TimeStamp),
        .CO2Delta =(int8_t)((int16_t)pEntry.CO2-(int16_t)_last->CO2),
        .TempDelta =(int8_t)(pEntry.Temp-(int16_t)_last->Temp),
        .HumidityDelta = (int8_t)(pEntry.Humidity-(int16_t)_last->Humidity),
        .PressureDelta = (int8_t)(pEntry.Pressure-(int16_t)_last->Pressure),            
    };

    _store->Write(pEntry.TimeStamp,(uint8_t *)&entry, sizeof(entry));        
}

void DataManager::WriteVeryCompactEntry(const DataEntry& pEntry){
    if(!_store->CanAddData(sizeof(DataPointVeryCompact))) 
    {
        printf("Flushing\n");
        if(_last) 
            WriteToFlash();
        
        _store->Flush(pEntry.TimeStamp);
        WriteFullEntry(pEntry);
    }

    DataPointVeryCompact entry = { 
        .Type = DataPointType::VeryCompact,  
        .TimeDelta = (uint)(pEntry.TimeStamp-_last->TimeStamp),
        .CO2Delta =(int8_t)((int16_t)pEntry.CO2-(int16_t)_last->CO2),
        .TempDelta =(int8_t)(pEntry.Temp-(int16_t)_last->Temp),
        .HumidityDelta = (int8_t)(pEntry.Humidity-(int16_t)_last->Humidity),
        .PressureDelta = (int8_t)(pEntry.Pressure-(int16_t)_last->Pressure),            
    };

    _store->Write(pEntry.TimeStamp,(uint8_t *)&entry, sizeof(entry));        
}

void DataManager::WriteEntry(const DataEntry& pEntry) {

    if(pEntry.TimeStamp < 1000000) return;

    if(pthread_mutex_lock(&_mutex) == 0){    
        auto entryType = !_last ? DataPointType::Full : GetRequiredBlockType(pEntry);
        switch(entryType) {
            case DataPointType::Full :
                printf("Writing Full Entry\n");
                WriteFullEntry(pEntry);
                break;
            case DataPointType::Compact :
                printf("Writing Compact Entry\n");
                WriteCompactEntry(pEntry);
                break;
            case DataPointType::VeryCompact :
                printf("Writing Very Compact Entry\n");
                WriteVeryCompactEntry(pEntry);
                break;
        }

        _written++;
        printf("Used %u, Written=%u\n",_store->GetUsed(),_written);
        if(!_last) _last = new DataEntry();
        *_last = pEntry;
       pthread_mutex_unlock(&_mutex);
    }
    
}


DataManagerQuery* DataManager::StartQuery(time_t pFrom, time_t pTo) {
     printf("DM Query %u->%u = %u\n", (uint)pFrom, (uint)pTo, (uint(pTo-pFrom)));
    return new DataManagerQuery(_mutex,*_flashStore, *_store, pFrom, pTo);
}

