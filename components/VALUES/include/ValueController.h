#pragma once
#include "Common.h"
#include<string>
#include<vector>
#include<map>
#include "SettingsBase.h"
#include<functional>
#include "AccessLevels.h"
#include "EnumFlags.h"

enum ValueMeasurement {
    // General 
    Generic = 0,
    StaticString = 1,
    DynamicString = 2,

    // Device Measurement
    BatteryVoltage = 100,
    ExternalPowerVoltage = 101,

    // Wifi Measurements
    WifiConnected = 200,
    WifiSignalStength = 201,
    WifiChannel = 202,

    // Atmospheric Measurements
    AtmosphericCO2 = 1001,
    AtmosphericTemperature,
    AtmosphericHumidity,
    AtmosphericPressure,
    AtmosphericPM25,
    AtmosphericVOC
};


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
    GET_LATEST_DATA = 4,
    CALIBRATION_INFO = 8
};

MAKE_ENUM_FLAGS(ValueSourceFlags);

class ValueSource {
    const ValuesSource& _valuesSource;
    const std::string _name;
    ValueMeasurement _measurement;
    ValueDataType _dataType;
    ValueUnit _unit;
    Value& _value;
    ValueSourceFlags _flags;
    uint _priority;
    bool _includeInMqttReadings;
    bool _includeInDataLog;


public:
    ValueSource(
        const ValuesSource& pValuesSource, 
        const std::string& pName, 
        ValueMeasurement pMeasurement, 
        ValueDataType pDataType, 
        ValueUnit pUnit, 
        Value& pValue,
        ValueSourceFlags pFlags = VALUESOURCE_NOFLAGS,
        uint pPriority = 0
        );

    const ValuesSource& GetValuesSource() const;
    const std::string& GetName() const;
    ValueMeasurement GetMeasurement() const;
    ValueDataType GetDataType() const;
    ValueUnit GetUnit() const;
    ValueSourceFlags GetFlags() const;
    uint GetPriority() const;
    inline bool IsIncludedInMQTTReadings() { return _includeInMqttReadings; } 
    inline bool IsIncludedInDataLog() { return _includeInMqttReadings; } 
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


class ValueController : public SettingsBase {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
    std::vector<ValuesSource*> _sources;
    std::map<const std::string, ValueSourceByNameHolder*> _sourcesByName;
    std::map<const std::string, MethodSource*> _methods;
    static ValueController* _current;
protected:
    const char* GetFilePath() override;
    void LoadSettingsFromJson(Json& pJson) override;
    void SaveSettingsToJson(Json& pJson) override;
public:
    void RegisterSource(ValuesSource* pSource);
    const std::vector<ValueSource*>& Find(const std::string& pName);
    ValueSource* GetDefault(const std::string& pName);
    void SetDefault(ValueSource* pValueSource);
    MethodSource* GetMethod(std::string pMethodGroup, std::string pMethodName);
    
    static ValueController& GetCurrent();


    inline std::map<const std::string, ValueSourceByNameHolder*>& GetSourcesByName() {
        return _sourcesByName;
    }





};


