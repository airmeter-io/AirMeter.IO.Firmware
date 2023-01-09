#include "ValueController.h"

ValueSource::ValueSource(const ValuesSource& pValuesSource, const std::string& pName, ValueMeasurement pMeasurement, ValueDataType pDataType, ValueUnit pUnit) :
    _valuesSource(pValuesSource), _name(pName), _measurement(pMeasurement), _dataType(pDataType), _unit(pUnit) {
    
}

const ValuesSource& ValueSource::GetValuesSource() const {
    return _valuesSource;
}

const std::string& ValueSource::GetName() const {
    return _name;
}

ValueMeasurement ValueSource::GetMeasurement() const {
    return _measurement;
}

ValueDataType ValueSource::GetDataType() const {
    return _dataType;
}

ValueUnit ValueSource::GetUnit() const {
    return _unit;
}

ValueController::ValueController() {

}

ValueController::~ValueController() {

}

void ValueController::RegisterSource(ValuesSource* pSource) {
    _sources.push_back(pSource);
    for(auto source : pSource->GetValueSources()) {
        if(_sourcesByName.find( source->GetName() ) == _sourcesByName.end()) {
            _sourcesByName[pSource->GetName()] = new ValueSourceByNameHolder();
            _sourcesByName[pSource->GetName()]->DefaultSource = source;
        }
        _sourcesByName[pSource->GetName()]->Sources.push_back(source);
    }
}

const std::vector<ValueSource*>& ValueController::Find(const std::string& pName) {
    return _sourcesByName[pName]->Sources;
}

ValueSource* ValueController::GetDefault(const std::string& pName) {
    return _sourcesByName[pName]->DefaultSource;
}

void ValueController::SetDefault(ValueSource* pValueSource) {
    _sourcesByName[pValueSource->GetName()]->DefaultSource = pValueSource;
}