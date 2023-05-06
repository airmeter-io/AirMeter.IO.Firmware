#pragma once

#include "Common.h"
#include "DataManagerStore.h"
#include "DataManagerStoreBucketReader.h"
#include "ValueController.h"
#include <vector>

class DataManagerQueryResults{
private:
    uint32_t _maxEntries;
    time_t* _timeStamps;
    int16_t* _values;
public: 
    inline DataManagerQueryResults(const std::vector<ValueSource*>& pValues, uint32_t pMaxEntries) : _maxEntries(pMaxEntries)  {
        _timeStamps = (time_t*)calloc(sizeof(time_t), _maxEntries);
        _values = (int16_t*)calloc(sizeof(int16_t)*pValues.size(), _maxEntries);
    }

    inline ~DataManagerQueryResults() {
        free(_timeStamps);
        free(_values);
    }

    inline time_t* GetTimeStamps() { return _timeStamps; }
    inline int16_t* GetValues() { return _values;}
    inline uint32_t GetMaxEntries() { return _maxEntries; }
} ;


class DataManagerQuery {

    DataManagerStore& _store;
    std::vector<DataStoreBucketInfo*> _buckets;
    const std::vector<ValueSource*>& _values;
    time_t _from;
    time_t _to;
    
    DataManagerStoreBucketReader* _reader  = nullptr;
    uint _bufferIndex  = 0;
    int _bucketIndex = 0;
    bool _loadedTemp = false;
    

public:
    DataManagerQuery(const std::vector<ValueSource*>& pValues, DataManagerStore& pStore, time_t pFrom, time_t pTo);
    ~DataManagerQuery();
    uint32_t ReadEntries(DataManagerQueryResults *pResults);
};