#include "ValueController.h"
#include "Json.h"

ValueSource::ValueSource(
    const ValuesSource& pValuesSource, 
    const std::string& pName, 
    ValueMeasurement pMeasurement, 
    ValueDataType pDataType, 
    ValueUnit pUnit, 
    Value& pValue,
    ValueSourceFlags pFlags,
    uint pPriority) :
    _valuesSource(pValuesSource), 
    _name(pName), 
    _measurement(pMeasurement), 
    _dataType(pDataType), 
    _unit(pUnit), 
    _value(pValue),
    _flags(pFlags),
    _includeInMqttReadings(pFlags & DEFAULT_MQTT),
    _includeInDataLog(pFlags & DEFAULT_DATALOG) {
    
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

ValueSourceFlags ValueSource::GetFlags() const {
    return _flags;
}

uint ValueSource::GetPriority() const {
    return _priority;
}

Value ValueSource::GetValue() {
    return _value;
}


MethodSource::MethodSource(
        const AccessLevels pRequiredAccessLevel,
        const ValuesSource& pValuesSource,
        const std::string& pGrouping,
        const std::string& pName,
        const std::vector<ValueDataType>& pParams,
        std::function<void(const std::vector<Value>& pValues)> pMethod) :
        _requiredAccessLevel(pRequiredAccessLevel),
        _valuesSource(pValuesSource),
        _grouping(pGrouping),
        _name(pName),
        _params(pParams),
        _method(pMethod) {

}

const std::string& MethodSource::GetGrouping() const {
    return _grouping;
}

const std::string& MethodSource::GetName() const {
    return _name;
}

const std::vector<ValueDataType>& MethodSource::GetParams() const {
    return _params;
}

void MethodSource::Invoke(const std::vector<Value>& pValues) {
    _method(pValues);
}


void ValuesSource::AddValueSource(ValueSource* pSource) {
    _sources.push_back(pSource);
}

void ValuesSource::AddMethodSource(MethodSource* pSource) {
    _methods.push_back(pSource);
}

void ValuesSource::RegisterWithValueController() {
    ValueController::GetCurrent().RegisterSource(this);
}

const std::vector<ValueSource*>& ValuesSource::GetValueSources() const {
    return _sources;
}

const std::vector<MethodSource*>& ValuesSource::GetMethodSources() const {
    return _methods;
}

void ValueController::RegisterSource(ValuesSource* pSource) {
    _sources.push_back(pSource);
    for(auto source : pSource->GetValueSources()) {
        if(_sourcesByName.find( source->GetName() ) == _sourcesByName.end()) {
            _sourcesByName[source->GetName()] = new ValueSourceByNameHolder();
            _sourcesByName[source->GetName()]->DefaultSource = source;
        }
        _sourcesByName[source->GetName()]->Sources.push_back(source);
    }
    for(auto method : pSource->GetMethodSources()) {
        auto fullName = method->GetGrouping()+"."+method->GetName();
        _methods[fullName] = method;
    }
}

const std::vector<ValueSource*>& ValueController::Find(const std::string& pName) {
    return _sourcesByName[pName]->Sources;
}


#define DEC_PLACE_MULTIPLIER 100

void ValueSource::SerialiseToJsonProperty(Json& pJson)
{
    switch(_dataType){
    case Bool : 
        pJson.CreateBoolProperty(_name, _value.b);
        break;
    case Int :
        pJson.CreateNumberProperty(_name, _value.i);
        break;
    case Double:  
        pJson.CreateNumberProperty(_name, _value.d);
        break;
    case String:
        pJson.CreateStringProperty(_name, *_value.s);
        break;
    case Fixed:
        char buf[20];
        auto value = _value.i;
        snprintf(buf, 
                 sizeof(buf), 
                 "%d.%.2d",  
                 (int)(value / DEC_PLACE_MULTIPLIER), 
                 (int)( abs(value) % DEC_PLACE_MULTIPLIER));
        pJson.CreateStringProperty(_name, buf);
        break;
    }
}

std::string ValueSource::GetValueAsString() {
    switch(_dataType){
    case Bool : 
        return _value.b ? "true" : "false";
    case Int :
        return std::to_string(_value.i);
    case Double:  
        return std::to_string(_value.d);
    case String:
        return *_value.s;        
    case Fixed:
        char buf[20];
        auto value = _value.i;
        snprintf(buf, 
                 sizeof(buf), 
                 "%d.%.2d",  
                 (int)(value / DEC_PLACE_MULTIPLIER), 
                 (int)( abs(value) % DEC_PLACE_MULTIPLIER));
        return buf;
    }
    return "-";
}

ValueSource* ValueController::GetDefault(const std::string& pName) {
    return _sourcesByName[pName]->DefaultSource;
}

void ValueController::SetDefault(ValueSource* pValueSource) {
    _sourcesByName[pValueSource->GetName()]->DefaultSource = pValueSource;
}


const char* ValueController::GetFilePath() {
    return "/spiffs/values.json";
}

void ValueController::LoadSettingsFromJson(Json& pJson) {
    for ( const auto &source : _sourcesByName ) {
        if(pJson.HasObjectProperty(source.first)) {
            auto jsonConfig = pJson.GetObjectProperty(source.first);
            auto valueByName = source.second;
            if(jsonConfig->HasProperty("Default")) {
                auto defStr = jsonConfig->GetStringProperty("Default");
                for(auto valSource : valueByName->Sources)
                    if(valSource->GetValuesSource().GetValuesSourceName()==defStr)
                    {
                        valueByName->DefaultSource = valSource;
                        break;
                    }                
            }
            delete jsonConfig;
        }
    }
}

void ValueController::SaveSettingsToJson(Json& pJson) {
    for(const auto &source : _sourcesByName) {
        auto jsonConfig = pJson.CreateObjectProperty(source.first);
        auto valueByName = source.second;
        jsonConfig.CreateStringProperty("Default", valueByName->DefaultSource->GetValuesSource().GetValuesSourceName());
    }
}

MethodSource* ValueController::GetMethod(std::string pMethodGroup, std::string pMethodName) {
    auto fullName = pMethodGroup+"."+pMethodName;
    if(_methods.count(fullName)==0) return nullptr;
    return _methods[fullName];
}

ValueController* ValueController::_current;

ValueController& ValueController::GetCurrent() {
    if(_current == nullptr)
        _current = new ValueController();
    return *_current;
}
