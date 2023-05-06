#pragma once
#include "Common.h"
#include<string>
#include<vector>
#include<map>
#include "SettingsBase.h"
#include<functional>
#include "AccessLevels.h"
#include "EnumFlags.h"

enum ValueDataType { Bool, Int, Fixed, Double, String};
enum ValueUnit { 
    Dimensionless, 
    Days,
    Hours,
    Minutes,
    Seconds,
    Centigrade, 
    Percent, 
    hPa, 
    PPM
};

typedef struct {
    uint16_t Id;
    const std::string Grouping;
    const std::string Name;
} ValueIdentifier;

typedef union {
    bool b;
    int i;
    struct {
        int n;
        uint16_t d;
    } f;
    double d;
    std::string* s;
} Value;

class ValuesSource;

enum ValueSourceFlags { 
    VALUESOURCE_NOFLAGS = 0,
    DEFAULT_DATALOG = 1, 
    DEFAULT_MQTT = 2,
    DEFAULT_MQTTINFO = 4,
    GET_LATEST_DATA = 8,
    CALIBRATION_INFO = 16,
    NETWORK_INFO = 32,
    DATALOG_AVAILABLE = 64
};

MAKE_ENUM_FLAGS(ValueSourceFlags);

class ValueSource {
    const ValuesSource& _valuesSource;
    const ValueIdentifier& _identifier;
    ValueDataType _dataType;
    ValueUnit _unit;
    Value& _value;
    ValueSourceFlags _flags;
    uint _priority;
    int _min;
    int _max;
    bool _includeInMqttReadings;
    bool _includeInMqttInfo;
    bool _includeInDataLog;
public:
    ValueSource(
        const ValuesSource& pValuesSource, 
        const ValueIdentifier& pIdentifier,
        ValueDataType pDataType, 
        ValueUnit pUnit, 
        Value& pValue,
        ValueSourceFlags pFlags = VALUESOURCE_NOFLAGS,
        int pMin = 0, 
        int pMax = 0,
        uint pPriority = 0
        );

    const ValuesSource& GetValuesSource() const;
    const ValueIdentifier& GetIdentifier() const;

    ValueDataType GetDataType() const;
    ValueUnit GetUnit() const;
    ValueSourceFlags GetFlags() const;
    uint GetPriority() const;
    inline bool IsIncludedInMQTTReadings() { return _includeInMqttReadings; } 
    inline bool IsIncludedInMQTTInfo() { return _includeInMqttInfo; } 
    inline bool IsIncludedInDataLog() { return _includeInDataLog; } 
    inline void SetIsIncludedInMQTTReadings(bool pIncluded) { _includeInMqttReadings = pIncluded; }
    inline void SetIsIncludedInMQTTInfo(bool pIncluded) { _includeInMqttInfo = pIncluded; }
    inline void SetIsIncludedInDatalog(bool pIncluded) { _includeInDataLog = pIncluded; }
    inline int GetMin() { return _min;}
    inline int GetMax() { return _max;}
    inline int GetRange() { return _max - _min; };
    inline int GetRequiredBits() {
        auto j = 0;
        for(auto i = GetRange(); i>0;  i>>=1, j++ );
        return j;
    }
    void SerialiseToJsonProperty(Json& pJson);
    Value GetValue();
    std::string GetValueAsString();
};

class MethodSource {
   const AccessLevels _requiredAccessLevel;
   const ValuesSource& _valuesSource; 
   const std::string _grouping;
   const std::string _name;
   const std::vector<ValueDataType> _params;
   std::function<void(const std::vector<Value>& pValues)> _method;
public:
    MethodSource(
        const AccessLevels pRequiredAccessLevel,
        const ValuesSource& pValuesSource,
        const std::string& pGrouping,
        const std::string& pName,
        const std::vector<ValueDataType>& pParams,
        std::function<void(const std::vector<Value>& pValues)> pMethod);

    const std::string& GetGrouping() const;
    const std::string& GetName() const;
    const std::vector<ValueDataType>& GetParams() const;
    void Invoke(const std::vector<Value>& pValues);
};

class ValuesSource {
private:
    std::vector<ValueSource*> _sources;
    std::vector<MethodSource*> _methods;
protected:
    void AddValueSource(ValueSource* pSource);
    void AddMethodSource(MethodSource* pSource);
public:
    void RegisterWithValueController();
    virtual const std::string& GetValuesSourceName() const = 0;
    const std::vector<ValueSource*>& GetValueSources() const;
    const std::vector<MethodSource*>& GetMethodSources() const;
};


class ValueSourceByNameHolder {
public:
    std::vector<ValueSource*> Sources;
    ValueSource* DefaultSource;
};

class ValueSourceGroupHolder {
public:
    std::map<const std::string, ValueSourceByNameHolder*> SourcesByName;
    std::map<const std::string, MethodSource*> Methods;
};


class ValueController : public SettingsBase {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
    std::vector<ValuesSource*> _sources;
    std::map<const std::string, ValueSourceGroupHolder*> _groups;
    std::map<uint16_t, ValueSource*> _sourceById;
    static ValueController* _current;
protected:
    const char* GetFilePath() override;
    void LoadSettingsFromJson(Json& pJson) override;
    void SaveSettingsToJson(Json& pJson) override;
public:
    void RegisterSource(ValuesSource* pSource);
    const std::vector<ValueSource*>& Find(const std::string& pGroup, const std::string& pName);
    ValueSource* GetDefault(const std::string& pGroup, const std::string& pName);
    void SetDefault(ValueSource* pValueSource);
    MethodSource* GetMethod(std::string pMethodGroup, std::string pMethodName);
    
    static ValueController& GetCurrent();
    ValueSource* GetSourceById(uint16_t pValueSourceId);

    inline std::map<const std::string, ValueSourceGroupHolder*>& GetGroups() {
        return _groups;
    }
};


