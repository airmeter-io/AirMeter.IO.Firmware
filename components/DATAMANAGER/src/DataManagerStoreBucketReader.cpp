#include "DataManagerStoreBucketReader.h"

DataManagerStoreBucketReader::DataManagerStoreBucketReader(const esp_partition_t*  pPartition, size_t pOffset, size_t pSize) :
     _partition(pPartition), _offset(pOffset), _size(pSize)  {
    _buffer = (uint8_t*)calloc(1, pSize);
    _header = (DataStoreBucketHeader *)_buffer;
    uint16_t *valueIds = (uint16_t*)(_buffer + sizeof(DataStoreBucketHeader));
    esp_partition_read(_partition, _offset ,_buffer, pSize);

    for(auto i = 0; i < _header->ValueCount; i++)
        _values.push_back(ValueController::GetCurrent().GetSourceById(valueIds[i]));
    _currentValues =  (int16_t*)calloc(sizeof(int16_t), _values.size());
    _headerSize = sizeof(DataStoreBucketHeader) + (sizeof(uint16_t)*_header->ValueCount);

    _reader = new BitReader(_buffer + _headerSize, _size - _headerSize );        
}

DataManagerStoreBucketReader::~DataManagerStoreBucketReader() {
    free(_buffer);
    free(_currentValues);
}

const std::vector<ValueSource*>& DataManagerStoreBucketReader::GetValues() const {
    return _values;
}

const int16_t* DataManagerStoreBucketReader::GetCurrentValues() const {
    return _currentValues;
}

bool DataManagerStoreBucketReader::ReadNext() {
    if(_reader->PeekByte()==0xFF) {
        printf("End of read\n");
        return false;
    }
    if(_reader->GetByteLength()+_headerSize>=_size) {
        printf("Out of data to read from bucket\n");
        return false;
    }
    auto bits = 0;
    switch(_reader->ReadUInt(3)) {
        case FullRecord :
        {
            auto timestampOffset = _reader->ReadUInt(29);
            _currentTime = _header->BlockStartTime + timestampOffset;
            _reader->ReadBytes((uint8_t*)_currentValues, _values.size()*sizeof(int16_t));
            
           // _reader->Advance();
            return true;
        }
        case FreeRecord :
            return false;
        case Delta3BitRecord :
            bits = 2;
            break;
        case Delta4BitRecord :
            bits = 3;
            break;
        case Delta5BitRecord : 
            bits = 4;
            break;
        case Delta8BitRecord :
            bits = 7;
            break;
        case Delta9BitRecord :
            bits = 8;
            break;
        case Delta11BitRecord :
            bits = 10;
            break;       
    }
    auto index = 0;
    auto repeat = true;
    uint32_t timeOffset = 0;
    while(repeat) {
        auto timeOff = _reader->ReadUInt(8);
        repeat = (timeOff & (1<<7));
        timeOffset |= (timeOff & ((1<<7)-1)) << index;
        index+=7;
    }
    _currentTime+=timeOffset;
    printf("Read values: ");
    for(auto i = 0; i<_values.size();i++) {
        printf("%d->",(int)(_currentValues[i]) );
        _currentValues[i]+=_reader->ReadInt(bits);
        printf("%d ",(int)(_currentValues[i]) );
    }
    printf("\n");
    _reader->Advance();
    return true;
}

uint32_t DataManagerStoreBucketReader::GetCurrentBlockOffset() {
    return _headerSize+_reader->GetByteLength();
}