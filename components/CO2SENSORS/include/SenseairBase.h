#pragma once
#include "Common.h"
#include "CO2Sensor.h"

enum MeterControlModes {
    nRDYEnabled = 1,
    ABCEnabled = 2,
    StaticIREnabled = 4,
    DynmaicIREnabled = 8,
    PressureCompensationEnabled = 16,
    nRDNYPinInvert = 32
};

class SenseairBase : public CO2Sensor {
protected:
    std::string _temp;
    std::string _measurementInfo;
    std::string _error;



    std::string _meterControl;
    Value _valFirmwareType = { .i = 0 };
    Value _valFirmwareTemperature = { .s = &_temp };
    Value _valMeasurementInfo = { .s = &_measurementInfo };
    Value _valError = { .s = &_error };   
    Value _valSingleMeasurementMode = { .b = false };
    Value _valMeasurementPeriod = { .i = 0 };
    Value _valMeasurementNoSamples = { .i = 0 };
    Value _valAbcPeriod = { .i = 0 };
    Value _valAbcTarget = { .i = 0 };
    Value _valFilter = { .i = 0 };
    Value _valMeterControl = { .s = &_meterControl };
    void UpdateErrorStatus(uint8_t* pInput);
public:
    SenseairBase();
   
};