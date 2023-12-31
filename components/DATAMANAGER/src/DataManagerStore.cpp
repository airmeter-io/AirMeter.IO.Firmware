#include "DataManagerStore.h"

#include "BitWriter.h"
#include "BitReader.h"
#include "DataManagerStorePrivate.h"

#define TAG "DataManagerStore"

DataManagerStore::DataManagerStore() {
    _partition = esp_partition_find_first((esp_partition_type_t)0x50,ESP_PARTITION_SUBTYPE_ANY, "readings");
    _numBuckets = _partition->size/BUCKET_SIZE;
    _buckets = (DataStoreBucketInfo*)calloc(sizeof(DataStoreBucketInfo), _numBuckets);
 //   EraseAll();
    if(_partition==nullptr) {
        printf("Failed to find readings partition\n");
        return;


    }
    printf("Readings partition found %u bytes\n", (unsigned int)_partition->size);
    
}

DataManagerStore::~DataManagerStore() {

}

void DataManagerStore::ScanBuckets() {
    _values.clear();
    for(auto groupPair : ValueController::GetCurrent().GetGroups())                
        for(auto keyPair : groupPair.second->SourcesByName) {
            auto source = keyPair.second->DefaultSource;
            if(source->IsIncludedInDataLog() ) {
                _values.push_back(source);
            }                         
        }       


    int32_t openBucketIndex = -1;
    int32_t maxCompletedBucketIndex = -1;
    for(uint32_t offset = 0; offset < _partition->size; offset+=BUCKET_SIZE) {
        DataManagerStoreBucket bucket(_partition, offset, BUCKET_SIZE);
        printf("%x: %d\n", (int)offset, (int)bucket.GetState());
        switch(bucket.GetState()) {
            case InvalidBucket :
                bucket.Erase();
                break;
            case UnusedBucket :
                break;
            case DataManagerStoreBucketState::OpenBucket :                
                if(openBucketIndex == -1 || bucket.GetHeader().BlockStartTime > _buckets[openBucketIndex].BlockStartTime) 
                {
                    openBucketIndex = offset/BUCKET_SIZE;
                }
                break;
            case CompletedBucket :
                if(maxCompletedBucketIndex ==-1 ||  bucket.GetHeader().BlockStartTime > _buckets[maxCompletedBucketIndex].BlockStartTime)
                    maxCompletedBucketIndex = bucket.GetHeader().Index;
                break;
            case IndexBucket :
                break;
        }
        auto header = bucket.GetHeader();
        _buckets[offset/BUCKET_SIZE]  = { .State = bucket.GetState(),  .Offset = offset, .Index = header.Index, .BlockStartTime = header.BlockStartTime, .BlockEndTime = header.BlockEndTime, .DataLength = header.DataLength, .NumReadings = header.NumReadings };
    
    }
   
    printf("Openning bucket, OpenBucketIndex  = %d, maxCompleted = %d\n",(int)openBucketIndex, (int)maxCompletedBucketIndex);
    OpenBucket(openBucketIndex!=-1 ? openBucketIndex : maxCompletedBucketIndex+1);
}

void DataManagerStore::FreeOldestBuckets() {
  /*  for(auto i = 0; i < _numBuckets; i++)
        if(_buckets[i].Index!=0xFFFFFFFF && 
            !(_buckets[i].BlockStartTime<pFrom && _buckets[i].BlockEndTime<pTo) &&
            !(_buckets[i].BlockStartTime>pFrom && _buckets[i].BlockEndTime>pTo))
            pResult.push_back(&_buckets[i]);*/
}

void DataManagerStore::EraseAll() {
   for(auto offset = 0; offset < _partition->size; offset+=BUCKET_SIZE) {
        DataManagerStoreBucket bucket(_partition, offset, BUCKET_SIZE);
        switch(bucket.GetState()) {
            case InvalidBucket :
            case  DataManagerStoreBucketState::OpenBucket :
            case CompletedBucket :
            case IndexBucket : 
                bucket.Erase();
                printf("Erasing %x\n", (int)offset);
                break;
            default:
                printf("Not Erasing %x\n", (int)offset);
                break;
        }
    } 
    OpenBucket(0);
}

void DataManagerStore::WriteRecord(time_t pTime){
    if(_values.size()==0) return;
    if(!_bucket->WriteRecord(pTime)) {
        OpenBucket(_currentBucket+1);
        _bucket->WriteRecord(pTime);
    }
}

void DataManagerStore::OpenBucket(uint32_t pIndex) {  
    if(_values.size()==0) return;
    if(pIndex>=_partition->size / BUCKET_SIZE)
        pIndex = 0;  
    if(_bucket!=nullptr) {
        auto header = _bucket->GetHeader();
        _buckets[_currentBucket] = { .State = _bucket->GetState(), .Offset = _currentBucket*BUCKET_SIZE, .Index = header.Index, .BlockStartTime = header.BlockStartTime, .BlockEndTime = header.BlockEndTime, .DataLength = header.DataLength, .NumReadings = header.NumReadings };
        delete _bucket;
    }
    _bucket = new DataManagerStoreBucket(_partition, pIndex*BUCKET_SIZE , BUCKET_SIZE);
    printf("Opening bucket values = %d\n", (int)_values.size());
    _bucket->Open(_values);
    _currentBucket = pIndex;
}

void DataManagerStore::GetBucketsForRange(time_t pFrom, time_t pTo, std::vector<DataStoreBucketInfo*>& pResult) {
    for(auto i = 0; i < _numBuckets; i++)
        if(_buckets[i].Index!=0xFFFFFFFF && 
            !(_buckets[i].BlockStartTime<pFrom && _buckets[i].BlockEndTime<pTo) &&
            !(_buckets[i].BlockStartTime>pFrom && _buckets[i].BlockEndTime>pTo))
            pResult.push_back(&_buckets[i]);
}

void DataManagerStore::GetCurrentBucketInfo(DataManagerStoreCurrentBucketInfo& pInfo) {
    auto header = _bucket->GetHeader();
    pInfo = {  
        .currentIndex = _currentBucket, 
        .numReadings = _bucket->GetNumReadings(), 
        .currentTime = _bucket->GetLastReading(), 
        .offset = _bucket->GetPayloadOffset(), 
        .size = BUCKET_SIZE,
        .info =  { .State = _bucket->GetState(), .Offset = _currentBucket*BUCKET_SIZE, .Index = header.Index, .BlockStartTime = header.BlockStartTime, .BlockEndTime = header.BlockEndTime, .DataLength = header.DataLength, .NumReadings = header.NumReadings } 
    };
}





