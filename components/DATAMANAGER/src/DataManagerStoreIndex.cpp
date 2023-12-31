#include "DataManagerStoreIndex.h"
#include "DataManagerStorePrivate.h"

void DataManagerStoreIndex::ReadHeader() {
    esp_partition_read(_partition, _offset , &_header, sizeof(_header));
    if(_header.Magic == INDEX_MAGIC) {
        _state = IndexBucket;
    } else if(_header.Magic != MAGIC) {
        if(_header.Magic == 0xFFFFFFFF)
            _state = UnusedBucket;
        else    
            _state = InvalidBucket;
    } else {
        if(_header.DataLength == 0xFFFFFFFF) {
            _state = OpenBucket;
        } else   
            _state = CompletedBucket;
    }
}

void DataManagerStoreIndex::WriteHeader() {
    esp_partition_write(_partition, _offset , &_header, sizeof(_header));
}

DataManagerStoreIndex::DataManagerStoreIndex(const esp_partition_t*  pPartition, size_t pOffset, size_t pSize) :
    _partition(pPartition), _offset(pOffset), _size(pSize) {
    ReadHeader();
}

DataManagerStoreIndex::~DataManagerStoreIndex() {

}

void DataManagerStoreIndex::Erase() {
    auto err = esp_partition_erase_range(_partition,_offset, BUCKET_SIZE);
    if (err != ESP_OK) 
        ESP_LOGE(TAG, "Fail to erase block (%d) %d, %d", err, (int)_offset, (int)BUCKET_SIZE); 
    ReadHeader();         
}


bool DataManagerStoreIndex::Open() {
    ReadHeader();
    if(_state == IndexBucket) {
        
    } else {
        if(_state!=UnusedBucket) 
            Erase();
        _header.Magic == INDEX_MAGIC;
        WriteHeader();    
    }
}