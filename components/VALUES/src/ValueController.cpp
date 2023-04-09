#include "ValueController.h"
#include "Json.h"

ValueSource::ValueSource(
    const ValuesSource& pValuesSource, 
    const ValueIdentifier& pIdentifier,
    ValueDataType pDataType, 
    ValueUnit pUnit, 
    Value& pValue,
    ValueSourceFlags pFlags,
    uint pPriority) :
    _valuesSource(pValuesSource), 
    _identifier(pIdentifier), 
    _dataType(pDataType), 
    _unit(pUnit), 
    _value(pValue),
    _flags(pFlags),
    _includeInMqttReadings(pFlags & DEFAULT_MQTT),
    _includeInMqttInfo(pFlags & DEFAULT_MQTTINFO),
    _includeInDataLog(pFlags & DEFAULT_DATALOG) {
    
}

const ValuesSource& ValueSource::GetValuesSource() const {
    return _valuesSource;
}

const ValueIdentifier& ValueSource::GetIdentifier() const {
    return _identifier;
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
        auto identifier = source->GetIdentifier();
        if(_groups.find(identifier.Grouping) == _groups.end()) {
            _groups[identifier.Grouping] = new ValueSourceGroupHolder();
        }
        auto group =_groups[identifier.Grouping];
        if(group->SourcesByName.find( identifier.Name) == group->SourcesByName.end()) {
            group->SourcesByName[identifier.Name] = new ValueSourceByNameHolder();
            group->SourcesByName[identifier.Name]->DefaultSource = source;
        }
        group->SourcesByName[identifier.Name]->Sources.push_back(source);
    }
    for(auto method : pSource->GetMethodSources()) {
        if(_groups.find(method->GetGrouping()) == _groups.end()) {
            _groups[method->GetGrouping()] = new ValueSourceGroupHolder();
        }
        auto group =_groups[method->GetGrouping()];
        group->Methods[method->GetName()] = method;
    }
}

const std::vector<ValueSource*> EmptyValueSourceList;

const std::vector<ValueSource*>& ValueController::Find(
    const std::string& pGroup, 
    const std::string& pName) {

    if(_groups.find(pGroup) == _groups.end()) 
        return EmptyValueSourceList;
    
    auto group =_groups[pGroup];
   
    return group->SourcesByName[pName]->Sources;
}


#define DEC_PLACE_MULTIPLIER 100

void ValueSource::SerialiseToJsonProperty(Json& pJson)
{
    switch(_dataType){
    case Bool : 
        pJson.CreateBoolProperty(_identifier.Name, _value.b);
        break;
    case Int :
        pJson.CreateNumberProperty(_identifier.Name, _value.i);
        break;
    case Double:  
        pJson.CreateNumberProperty(_identifier.Name, _value.d);
        break;
    case String:
        pJson.CreateStringProperty(_identifier.Name, *_value.s);
        break;
    case Fixed:
        char buf[20];
        auto value = _value.i;
        snprintf(buf, 
                 sizeof(buf), 
                 "%d.%.2d",  
                 (int)(value / DEC_PLACE_MULTIPLIER), 
                 (int)( abs(value) % DEC_PLACE_MULTIPLIER));
        pJson.CreateStringProperty(_identifier.Name, buf);
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


ValueSource* ValueController::GetDefault(const std::string& pGroup, const std::string& pName) {
    if(_groups.find(pGroup) == _groups.end()) 
        return nullptr;
    
    auto group =_groups[pGroup];   
    return group->SourcesByName[pName]->DefaultSource;
}

void ValueController::SetDefault(ValueSource* pValueSource) {

    if(_groups.find(pValueSource->GetIdentifier().Grouping) == _groups.end()) 
        return;
    
    auto group =_groups[pValueSource->GetIdentifier().Grouping];   
    group->SourcesByName[pValueSource->GetIdentifier().Name]->DefaultSource = pValueSource;
}

   


const char* ValueController::GetFilePath() {
    return "/spiffs/values.json";
}

void ValueController::LoadSettingsFromJson(Json& pJson) {
    for( const auto &group : _groups) {
        if(pJson.HasObjectProperty(group.first)) {
            auto groupJson = pJson.GetObjectProperty(group.first);

            for ( const auto &source : group.second->SourcesByName ) {
                if(groupJson->HasObjectProperty(source.first)) {
                    auto jsonConfig = groupJson->GetObjectProperty(source.first);
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
                    for(const auto valueSource : valueByName->Sources)
                    {
                        if(jsonConfig->HasObjectProperty(valueSource->GetValuesSource().GetValuesSourceName())) {
                            auto sourceConfig = jsonConfig->GetObjectProperty(valueSource->GetValuesSource().GetValuesSourceName());

                            if(sourceConfig->HasProperty("Log"))
                                valueSource->SetIsIncludedInDatalog(sourceConfig->GetBoolProperty("Log"));

                            if(sourceConfig->HasProperty("Mqtt"))
                                valueSource->SetIsIncludedInMQTTReadings(sourceConfig->GetBoolProperty("Mqtt"));

                            if(sourceConfig->HasProperty("MqttInfo"))
                                valueSource->SetIsIncludedInMQTTInfo(sourceConfig->GetBoolProperty("MqttInfo"));

                            delete sourceConfig;
                        } 
                    }
                    delete jsonConfig;
                } 
            }
            delete groupJson;
        }         
    }
}

void ValueController::SaveSettingsToJson(Json& pJson) {
    for( const auto &group : _groups) {
        auto groupConfig = pJson.CreateObjectProperty(group.first);
        for(const auto &source : group.second->SourcesByName) {
            auto jsonConfig = groupConfig->CreateObjectProperty(source.first);
            auto valueByName = source.second;
            
            jsonConfig->CreateStringProperty("Default", valueByName->DefaultSource->GetValuesSource().GetValuesSourceName());
           
            for(const auto valueSource : valueByName->Sources)
            {
                auto sourceConfig = jsonConfig->CreateObjectProperty(valueSource->GetValuesSource().GetValuesSourceName());
                sourceConfig->CreateBoolProperty("Log", valueSource->IsIncludedInDataLog());
                sourceConfig->CreateBoolProperty("Mqtt", valueSource->IsIncludedInMQTTReadings());
                sourceConfig->CreateBoolProperty("MqttInfo", valueSource->IsIncludedInMQTTInfo());
                delete sourceConfig;
            }
             delete jsonConfig;
     
        }
        delete groupConfig;
    }
}

MethodSource* ValueController::GetMethod(std::string pMethodGroup, std::string pMethodName) {
    if(_groups.find(pMethodGroup) == _groups.end()) 
        return nullptr;
    
    auto group =_groups[pMethodGroup];   
    return  group->Methods[pMethodName];
}

ValueController* ValueController::_current;

ValueController& ValueController::GetCurrent() {
    if(_current == nullptr)
        _current = new ValueController();
    return *_current;
}
