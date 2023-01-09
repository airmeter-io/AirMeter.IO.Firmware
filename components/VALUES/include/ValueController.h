#pragma once
#include "Common.h"
#include<string>
#include<vector>
#include<map>


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
    AtmosphericCO2Unfiltered,
    AtmosphericTemperature,
    AtmosphericHumidity,
    AtmosphericPressure,
    AtmosphericPM25,
    AtmosphericVOC
};

enum ValueDataType { Bool, Int, Fixed, Double, String};
enum ValueUnit { None, Centigrade, Percent, hPa, PPM };
class Value {
public:
   
    union {
        bool b;
        int i;
        struct {
          int n;
          uint16_t d;
        } f;
        double d;
        std::string* s;
    } value;
    std::string name;
    int priority;
};

class ValuesSource;

class ValueSource {
    const ValuesSource& _valuesSource;
    const std::string _name;
    ValueMeasurement _measurement;
    ValueDataType _dataType;
    ValueUnit _unit;
    
public:
    ValueSource(const ValuesSource& pValuesSource, const std::string& pName, ValueMeasurement pMeasurement, ValueDataType pDataType, ValueUnit pUnit);

    const ValuesSource& GetValuesSource() const;
    const std::string& GetName() const;
    ValueMeasurement GetMeasurement() const;
    ValueDataType GetDataType() const;
    ValueUnit GetUnit() const;

    virtual Value& GetValue()  = 0;
};


class ValuesSource {
public:
    virtual const std::string& GetName() const = 0;
    virtual const std::vector<ValueSource*>& GetValueSources() const = 0;
};

class ValueSourceByNameHolder {
public:
    std::vector<ValueSource*> Sources;
    ValueSource* DefaultSource;
};

class ValueController {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
    std::vector<ValuesSource*> _sources;
    std::map<const std::string, ValueSourceByNameHolder*> _sourcesByName;
public:
    ValueController();
    ~ValueController();
    void RegisterSource(ValuesSource* pSource);
    const std::vector<ValueSource*>& Find(const std::string& pName);
    ValueSource* GetDefault(const std::string& pName);
    void SetDefault(ValueSource* pValueSource);
};