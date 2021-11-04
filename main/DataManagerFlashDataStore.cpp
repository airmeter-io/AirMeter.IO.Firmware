#include "DataManagerFlashDataStore.h"
#include "DataManagerPrivate.h"

#define MAGIC 0x392F389a

#define BUCKET_SIZE 4096

DataManagerFlashDataBucket::DataManagerFlashDataBucket() {
    _buffer = (uint8_t*)calloc(BUCKET_SIZE, 1);
}

DataManagerFlashDataBucket::~DataManagerFlashDataBucket() {
    free(_buffer);
}

FlashDataBlockHeader* DataManagerFlashDataBucket::GetHeader() {
    return (FlashDataBlockHeader*)_buffer;
}

uint DataManagerFlashDataBucket::GetSize() {
    return BUCKET_SIZE;
}

uint DataManagerFlashDataBucket::GetPayloadSize() {
    return BUCKET_SIZE-sizeof(FlashDataBlockHeader);
}

uint8_t* DataManagerFlashDataBucket::GetPayload() {
    return _buffer+sizeof(FlashDataBlockHeader);
}

DataManagerFlashDataStore::DataManagerFlashDataStore() {
    _partition = esp_partition_find_first((esp_partition_type_t)0x50,ESP_PARTITION_SUBTYPE_ANY, "readings");
    if(_partition==nullptr) {
        printf("Failed to find readings partition\n");
        return;


    }
    printf("Readings partition found %u bytes\n", _partition->size);
    ScanBuckets();
}

DataManagerFlashDataStore::~DataManagerFlashDataStore() {

}
    
int DataManagerFlashDataStore::CalcPrevious(int pCurrent) {
    if(pCurrent == -1) return 0;
    if(pCurrent==0) return GetBucketCount()-1;
    return pCurrent-1; 
}

int DataManagerFlashDataStore::CalcNext(int pCurrent) {
    if(pCurrent == -1) return 0;
    if(pCurrent+1 == GetBucketCount())
        return 0;
    return pCurrent+1;
}

void DataManagerFlashDataStore::ScanBuckets() {
    DataManagerFlashDataBucket bucket;

    FlashDataBlockHeader latest;
    latest.BlockEndTime = 0;

    printf("Scanning buckets\n");
    for(auto i = 0; i < GetBucketCount(); i++)
    {

        if(!LoadBucket(i,&bucket))
            printf("Failed to load bucket %d\n",i);
        
        if(bucket.GetHeader()->Magic == MAGIC) { 
            if(_latestValid==-1) {
                _latestValid = i;
                latest = *bucket.GetHeader();
            } else {
                if(latest.BlockEndTime<bucket.GetHeader()->BlockEndTime) {
                    _latestValid = i;
                    latest = *bucket.GetHeader();
                }
            }            
        }
    }

    if(_latestValid!=-1) {
        _oldestValid = _latestValid;
        for(auto i  = CalcPrevious(_latestValid); i!=_latestValid; i=CalcPrevious(i) ) {
            printf("Oldest.. %d\n", i);
            if(!LoadBucket(i, &bucket))
                printf("Failed to load bucket %d\n",i);
            if(bucket.GetHeader()->Magic == MAGIC) {
                if(latest.BlockEndTime>bucket.GetHeader()->BlockEndTime) {
                    _oldestValid = i;
                    latest = *bucket.GetHeader();
                } else {
                    break;
                }
            } else
                break;
        }
    }

    
    printf("Done scanning buckets, Latest=%d, Oldest=%d\n", _latestValid, _oldestValid);

}

uint DataManagerFlashDataStore::GetBucketCount() const {
    return _partition->size/GetBucketSize();
}

bool DataManagerFlashDataStore::LoadBucket(uint pIndex, DataManagerFlashDataBucket* pBucket) {
    if(pBucket->GetSize()<GetBucketSize()) return false;
    if(pIndex>=GetBucketCount()) return false;

    auto err = esp_partition_read(_partition, pIndex*GetBucketSize(),pBucket->GetHeader(), GetBucketSize());
    if(err!=ESP_OK) {
        printf("Error Occurred Loading Data Reading Flash Bucket: %d\n", err);
        return false;
    }

    return true;
}
uint DataManagerFlashDataStore::GetBucketSize() const {
    return 4096;
}
void DataManagerFlashDataStore::SaveBucket( DataManagerFlashDataBucket* pBucket) {
    pBucket->GetHeader()->Magic = MAGIC;
    time(&pBucket->GetHeader()->WriteTime);
    auto next = CalcNext(_latestValid);
    auto err = esp_partition_write(_partition,next*GetBucketSize(),pBucket->GetHeader(), GetBucketSize());
    if(err!=ESP_OK) {
        printf("Error Occurred LoadingWriting Flash Bucket: %d\n", err);
        return ;
    }

    if(_latestValid==-1 || _oldestValid == -1)  {
        _latestValid = next;
        _oldestValid = next;
    } else {
        _latestValid = next;
        if(next==_oldestValid) {
            _oldestValid = CalcPrevious(_oldestValid);
        }
    }

    printf("Use after save, Latest=%d, Oldest=%d\n", _latestValid, _oldestValid);
        

}

int DataManagerFlashDataStore::GetOldest() {
    return _oldestValid;
}

int DataManagerFlashDataStore::GetLatest() {
    return _latestValid;
}