#pragma once
#include "Common.h"
#include "CO2Sensor.h"
#include <vector>

enum MeterControlModes {
    nRDYEnabled = 1,
    ABCEnabled = 2,
    StaticIREnabled = 4,
    DynmaicIREnabled = 8,
    PressureCompensationEnabled = 16,
    nRDNYPinInvert = 32
};

class SenseairBase : public CO2Sensor {
    const uint16_t UnfilteredWindowSize = 5;
    std::vector<uint16_t> _previousUnfilteredValues;

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
    Value _valAbcTarget = { .i = 0 };
    Value _valFilter = { .i = 0 };
    Value _valMeterControl = { .s = &_meterControl };
    Value _valCO2Smoothed = {.i = 400 };
    void UpdateErrorStatus(uint8_t* pInput);
    void UpdateSmoothed(uint16_t pUnfilteredValue);
public:
    SenseairBase();
   
};