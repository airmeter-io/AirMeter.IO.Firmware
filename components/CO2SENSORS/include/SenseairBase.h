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
    std::string _serialNo;
    std::string _softVer;
    std::string _deviceName;
    std::string _temp;
    std::string _measurementInfo;
    std::string _error;

    int _ppm = 400;
    bool _isHeatingUp = false;
    bool _sensorError = false;
    bool _abcEnabled = true;
    static const std::vector<int> availablePPMs;
    std::string _meterControl;
    CO2SensorValue _valFirmwareType = { .type = Int, .value = { .i = 0}, .name = "Firmware Type" };
    CO2SensorValue _valFirmwareTemperature = { .type = String, .value = { .s = &_temp}, .name = "Temperature" };
    CO2SensorValue _valMeasurementInfo = { .type = String, .value = { .s = &_measurementInfo}, .name = "Measurement Info" };
    CO2SensorValue _valError = { .type = String, .value = { .s = &_error}, .name = "Errors" };   
    CO2SensorValue _valSingleMeasurementMode = { .type = Bool, .value = { .b = false}, .name = "Single Measurement Mode" };
    CO2SensorValue _valMeasurementPeriod = { .type = Int, .value = { .i = 0}, .name = "Measurement Period" };
    CO2SensorValue _valMeasurementNoSamples = { .type = Int, .value = { .i = 0}, .name = "Measurement Num Samples" };
    CO2SensorValue _valAbcPeriod = { .type = Int, .value = { .i = 0}, .name = "ABC Period" };
    CO2SensorValue _valAbcTarget = { .type = Int, .value = { .i = 0}, .name = "ABC Target" };
    CO2SensorValue _valFilter = { .type = Int, .value = { .i = 0}, .name = "Static IR Filter" };
    CO2SensorValue _valMeterControl = { .type = String, .value = { .s = &_meterControl}, .name = "Meter Control" };
    void UpdateErrorStatus(uint8_t* pInput);
public:
    SenseairBase();
    
    std::string& GetSerialNo() override;    
    std::string& GetDeviceName() override;
    std::string& GetSWVersion() override;
    int GetPPM() override;
    int GetMaxPPM() override;
    const std::vector<int>& GetAvailableMaxPPM() const override;
    void SetMaxPPM(int pMaxPPM) override;    
    bool GetIsHeatingUp() override;
    bool GetHasError() override;
    int GetBasePPM() override;
    int GetDaysPerABCCycle() override;
    bool GetIsABCEnabled() override;
};