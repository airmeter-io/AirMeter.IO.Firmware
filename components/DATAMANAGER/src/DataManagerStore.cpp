#include "DataManagerStore.h"

#include "BitWriter.h"
#include "BitReader.h"
#include "DataManagerStorePrivate.h"

#define TAG "DataManagerStore"

DataManagerStore::DataManagerStore() {
    for(auto groupPair : ValueController::GetCurrent().GetGroups())                
        for(auto keyPair : groupPair.second->SourcesByName) {
            auto source = keyPair.second->DefaultSource;
            if(source->IsIncludedInDataLog() ) {
                _values.push_back(source);
            }                         
        }       
    ScanBuckets();
}

DataManagerStore::~DataManagerStore() {

}

void DataManagerStore::ScanBuckets() {
    int openBucketOffset = -1;
    uint32_t openBucketIndex = 0;
    uint32_t maxCompletedBucketIndex = 0;
    
    for(uint32_t offset = 0; offset < _partition->size; offset+=BUCKET_SIZE) {
        DataManagerStoreBucket bucket(_partition, offset, BUCKET_SIZE);
        switch(bucket.GetState()) {
            case InvalidBucket :
                bucket.Erase();
                break;
            case UnusedBucket :
                break;
            case DataManagerStoreBucketState::OpenBucket :                
                if(openBucketOffset == -1 || bucket.GetHeader()->Index > openBucketIndex) 
                {
                    openBucketOffset = offset;
                    openBucketIndex = bucket.GetHeader()->Index;
                }
                break;
            case CompletedBucket :
                if(bucket.GetHeader()->Index > maxCompletedBucketIndex)
                    maxCompletedBucketIndex = bucket.GetHeader()->Index;
                break;
        }
        auto header = bucket.GetHeader();
        _buckets[offset/BUCKET_SIZE]  = { .Offset = offset, .Index = header->Index, .BlockStartTime = header->BlockStartTime, .BlockEndTime = header->BlockEndTime, .DataLength = header->DataLength };
    }
    if(openBucketOffset!=-1 && maxCompletedBucketIndex>=openBucketIndex) {
        ESP_LOGE(TAG, "Readings partition is corrupt. The max completed bucket index (%d) is greater than the open bucket index (%d). Erasing all buckets.", (int)maxCompletedBucketIndex, (int)openBucketIndex ); 
        EraseAll();
        return;
    }
    OpenBucket(openBucketIndex!=-1 ? openBucketIndex : maxCompletedBucketIndex+1);
}

void DataManagerStore::EraseAll() {
   for(auto offset = 0; offset < _partition->size; offset+=BUCKET_SIZE) {
        DataManagerStoreBucket bucket(_partition, offset, BUCKET_SIZE);
        switch(bucket.GetState()) {
            case InvalidBucket :
            case  DataManagerStoreBucketState::OpenBucket :
            case CompletedBucket :
                bucket.Erase();
                break;
            default:
                break;
        }
    } 
    OpenBucket(0);
}

void DataManagerStore::WriteRecord() {
    if(!_bucket->WriteRecord()) {
        OpenBucket(_currentBucket+1);
        _bucket->WriteRecord();
    }
}

void DataManagerStore::OpenBucket(uint32_t pIndex) {  
    if(pIndex>=_partition->size / BUCKET_SIZE)
        pIndex = 0;  
    if(_bucket!=nullptr)
        delete _bucket;
    _bucket = new DataManagerStoreBucket(_partition, pIndex*BUCKET_SIZE , BUCKET_SIZE);
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





