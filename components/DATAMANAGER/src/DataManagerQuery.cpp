#include "DataManagerQuery.h"

#include "DataManagerStorePrivate.h"

   
DataManagerQuery::DataManagerQuery(const std::vector<ValueSource*>& pValues, DataManagerStore& pStore, time_t pFrom, time_t pTo) : 
    _store(pStore),_values(pValues), _from(pFrom), _to(pTo) {   
    pStore.GetBucketsForRange(pFrom, pTo, _buckets);
}

DataManagerQuery::~DataManagerQuery() {
    if(_reader)
        delete _reader;
}

 uint32_t DataManagerQuery::ReadEntries(DataManagerQueryResults *pResults) { 
    std::map<uint32_t, uint32_t> mappings;
    auto resultRow = 0;
    for(; resultRow<pResults->GetMaxEntries(); ) {
        while(_reader == nullptr && _bucketIndex<_buckets.size()) {
            if(_reader==nullptr) {
                _reader = new DataManagerStoreBucketReader(_store.GetPartition(), _buckets[_bucketIndex]->Offset,BUCKET_SIZE);
            }
            auto readerIndex = 0;
            mappings.clear();
            for(auto value : _reader->GetValues()) {                
                auto requiredIndex = 0;
                for(auto requiredVal : _values) {                     
                    if(value->GetIdentifier().Id == requiredVal->GetIdentifier().Id)
                        mappings[requiredIndex] = readerIndex;   
                    requiredIndex++;
                }
                readerIndex++;
            }
            if(mappings.size()==0) {
                delete _reader;
                _reader = nullptr;
                _bucketIndex++;
            }               
        }

        if(_reader==nullptr) break;

        auto time =  _reader->GetCurrentTime();
        if(time >= _from && time <= _to) {
            pResults->GetTimeStamps()[resultRow] = time;
            auto values = pResults->GetValues();
            auto readerValues = _reader->GetCurrentValues();
            for(auto i = 0; i < _values.size(); i++) {
                values[resultRow*_values.size()*sizeof(int16_t)] = mappings.contains(i) ? readerValues[mappings[i]] : 0;
            }
            resultRow++;
        }
    }

    return resultRow;
}