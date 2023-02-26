#pragma once

#include<string>
#include<vector>
#include "ValueController.h"
#include "CommonValueNames.h"



const std::string CO2METHOD_CALIBRATE = "Calibrate";
const std::string CO2METHOD_ENABLEABC = "EnableABC";
const std::string CO2METHOD_DISABLEABC = "DisableABC";
const std::string CO2METHOD_SETMAXPPM = "SetMaxPPM";



class CO2Sensor : public ValuesSource {
protected:
    std::string _serialNo = "-";
    std::string _softwareVersion = "-";
    std::string _deviceName = "-";
private:
    Value _valSerialNo { .s = &_serialNo };
    Value _valDeviceName { .s = &_deviceName };
    Value _valSwVersion { .s = &_softwareVersion };
protected:
    Value _valCo2 = { .i = 0 };
    Value _valCo2Unfiltered = {.i = 0};
    Value _valCo2Uncompensated { .i = 0};
    Value _valCo2UnfilteredUncompensated { .i = 0};
    Value _valMaxPPM { .i = 2000 };
    Value _valMaxBasePPM { .i = 2000};
    Value _valMinBasePPM { .i = 400 };
    Value _valBasePPM { .i = 400 };
    Value _valHasError { .b = false };
    Value _valIsHeatingUp { .b = false } ;
    Value _valIsAbcEnabled {.b = false };
    Value _valHoursPerAbcCycle;
    Value _valMinHoursPerAbcCycle { .i = 1 };
    Value _valMaxHoursPerAbcCycle { .i = 31*24};
    Value _valStepHoursPerAbcCycle { .i = 1};
    Value _valCalibWaitTime { .i = 2*60 };
public:
    CO2Sensor();

    virtual bool RefreshValues() = 0;
  
    virtual void DisableABC() = 0;
    virtual void ManualCalibration(int pBaseLinePPM) = 0;
    virtual void EnableABC(int pBaseLinePPM, int pNumberOfHoursPerCycle) = 0;
};

class ISensorManager {
public:
    virtual void EnableSensorReadGPIO() = 0;
    virtual void DisableSensorReadGPIO() = 0;
};