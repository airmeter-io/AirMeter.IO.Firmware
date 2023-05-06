#include "DataManagerStoreBucket.h"
#include "DataManagerStoreBucketReader.h"
#include "DataManagerStorePrivate.h"
#define TAG "DataManagerStoreBucket"
#include "BitWriter.h"


DataManagerStoreBucket::DataManagerStoreBucket(const esp_partition_t*  pPartition, size_t pOffset, size_t pSize) : _partition(pPartition), _offset(pOffset), _size(pSize) {
   ReadHeader();
}


DataManagerStoreBucket::~DataManagerStoreBucket() {
}

DataStoreBucketHeader* DataManagerStoreBucket::GetHeader() {
    return &_header;
}

DataManagerStoreBucketState DataManagerStoreBucket::GetState() {
    return _state;
}
void DataManagerStoreBucket::ReadHeader() {
    esp_partition_read(_partition, _offset , &_header, sizeof(_header));
    if(_header.Magic != MAGIC) {
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

void DataManagerStoreBucket::CloseBucket() {
    ReadHeader();
    _header.DataLength = _offset - sizeof(_header) - (_header.ValueCount*sizeof(uint16_t));
    _header.BlockEndTime = _lastReading;
    auto closeTime = time(nullptr);
    _header.CloseTime = closeTime;
    _header.NumReadings = _numReadings;
    WriteHeader();
    ReadHeader();
}

bool DataManagerStoreBucket::WriteRecord(uint8_t* pData, uint32_t pDataLength) {
    if(pDataLength+_payloadOffset>BUCKET_SIZE)
    {
        CloseBucket();
        return false;
    }
    esp_partition_write(_partition, _offset+_payloadOffset , pData, pDataLength);
    _payloadOffset+=pDataLength;

    return true;
}

void DataManagerStoreBucket::WriteHeader() {
    esp_partition_write(_partition, _offset , &_header, sizeof(_header));
}

bool DataManagerStoreBucket::Open(const std::vector<ValueSource*>& pValues){
    _values =  pValues;
    if(_state == UnusedBucket) {
        ReadHeader();    
        _header.Magic = MAGIC;
        _header.ValueCount = pValues.size();  
        auto openTime = time(nullptr);
        _header.OpenTime = openTime;
        WriteHeader();
        auto valueIdSize = _header.ValueCount*sizeof(uint16_t);
        uint16_t* valueIds = (uint16_t*)malloc(valueIdSize);
        _lastValues = (int16_t*)calloc(sizeof(int16_t), pValues.size());
        for(auto i = 0; i < _header.ValueCount; i++)
            valueIds[i] = pValues[i]->GetIdentifier().Id;
        _payloadOffset = sizeof(_header);
        esp_partition_write(_partition, _offset+_payloadOffset , valueIds, valueIdSize);
        _payloadOffset += valueIdSize;
        ReadHeader();
        _firstRecord = true;
        _numReadings = 0;
        return true;
    } else if (_state == OpenBucket) {
        DataManagerStoreBucketReader reader(_partition, _offset, _size);
        _firstRecord = true;
        _numReadings = 0;
        while(reader.ReadNext()) {
            _numReadings++;
            _firstRecord = false;
        }
        if(!_firstRecord) {
            _lastReading  = reader.GetCurrentTime();
            memcpy(_lastValues, reader.GetCurrentValues(), sizeof(uint16_t)*_values.size());            
        }
        _payloadOffset = reader.GetCurrentBlockOffset();

        return true;
    }
    return false;
}

void DataManagerStoreBucket::Erase() {
    auto err = esp_partition_erase_range(_partition,_offset, BUCKET_SIZE);
    if (err != ESP_OK) 
        ESP_LOGE(TAG, "Fail to erase block (%d) %d, %d", err, (int)_offset, (int)BUCKET_SIZE); 
    ReadHeader();         
}


void DataManagerStoreBucket::GetCurrentValues(int16_t* pValues) {
    for(auto value  : _values) {
        auto curVal = value->GetValue();
        switch(value->GetDataType()) {
            case Bool :
                *pValues = (int16_t)curVal.b;
                break;
            case Int :
                *pValues = (int16_t)curVal.i;
                break;
            case Fixed :
                *pValues = (int16_t)curVal.f.n;
                break;
            case Double :
                break;
            case String :
                break;
        }
        pValues++;
    }
}

bool DataManagerStoreBucket::WriteDeltaRecord() {
    int16_t* values = (int16_t*)calloc(sizeof(int16_t), _values.size());
    uint8_t* record = (uint8_t*)malloc(_values.size()*2);
    BitWriter writer(record, _values.size()*2);
    GetCurrentValues(values);
    auto readingTime = time(NULL);

    auto offset = readingTime - _lastReading;

    int32_t max=0;
    for(auto i = 0; i < _values.size(); i++) {
        values[i] -= _lastValues[i];       
        if(abs(values[i])>max)
            max = abs(values[i]);
    }
    
    auto result = false;
    auto recordType = FullRecord;
    auto bits = 0;

    if(max < (1 << 2)) {
        recordType = Delta3BitRecord;
        bits = 2;
    } else if (max < (1<<4)) {
        recordType = Delta5BitRecord;
        bits = 4;
    } else if (max < (1<<6)) {
        recordType = Delta7BitRecord;
        bits = 6;
    } else if (max < (1<<10)) {
        recordType = Delta11BitRecord;
        bits = 10;
    } 

    if(recordType!=FullRecord) {
        writer.WriteUInt((uint32_t)recordType, 3);
        while(offset>0) {
            uint8_t cur = offset & ((1 << 7)-1);
            offset >>= 7;
            if(offset)
                cur |= 1 << 7;
            writer.WriteUInt((uint32_t)cur, 8);
        }
        for(auto i = 0; i < _values.size(); i++) {
            writer.WriteInt(values[i], bits);
        }
        result = WriteRecord(record, writer.GetByteLength());
        
        memcpy(_lastValues, values, _values.size()*sizeof(int16_t));
    } else
        result = WriteFullRecord();

    free(values);
    free(record);
    if(result)
        _lastReading = readingTime;
    return result;
}

bool DataManagerStoreBucket::WriteFullRecord() {
    auto time = time_t(nullptr);
    if(_header.BlockStartTime> time) {
        ESP_LOGE(TAG, "Current time is before block start time"); 
        return false;
    }
    auto timeOffset = (uint32_t)(time - _header.BlockStartTime);
    auto recordSize = sizeof(uint32_t) + _values.size()*2;
    uint8_t* record = (uint8_t*)calloc(1, recordSize);
    
    _lastReading = time;
    BitWriter bitWriter(record, 4);
    bitWriter.WriteUInt((uint32_t)FullRecord, 3);
    bitWriter.WriteUInt(timeOffset, 29);    
    int16_t* values = (int16_t*)(record + sizeof(uint32_t));
    GetCurrentValues(_lastValues);
    memcpy(values, _lastValues, sizeof(int16_t)*_values.size());   

    auto result = WriteRecord(record, recordSize);

    free(record);
    return result;
}

bool DataManagerStoreBucket::WriteRecord() {
    auto result = false;
    if(_firstRecord)
    {
        result = WriteFullRecord();
        if(result) {
            if(_header.BlockStartTime == 0xFFFFFFFFFFFFFFFF ) {
                ReadHeader();
                _header.BlockStartTime = _lastReading;
                WriteHeader();
            }
            _firstRecord = false;
        }
       
    } else 
        result = WriteDeltaRecord();

    if(result) _numReadings++;
    return result;
}



