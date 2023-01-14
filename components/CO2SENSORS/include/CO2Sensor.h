#pragma once

#include<string>
#include<vector>
#include "ValueController.h"

const std::string CO2VALUE = "CO2";
const std::string CO2VALUE_UNFILTERED = "CO2_UNFILTERED";
const std::string CO2VALUE_UNCOMPENSATED = "CO2_UNCOMPENSATED";
const std::string CO2VALUE_UNFILTERED_UNCOMPENSATED = "CO2_UNFILTERED_UNFILTERED";
const std::string CO2VALUE_MAXPPM = "CO2_MaxPPM";
const std::string CO2VALUE_BASEPPM = "CO2_BasePPM";
const std::string CO2VALUE_MINBASEPPM = "CO2_MinBasePPM";
const std::string CO2VALUE_MAXBASEPPM = "CO2_MaxBasePPM";
const std::string CO2VALUE_HASERROR = "CO2_HasError";
const std::string CO2VALUE_ISHEATING = "CO2_IsHeating";
const std::string CO2VALUE_ISABCENABLED = "CO2_IsABCEnabled";
const std::string CO2VALUE_DAYSPERCYCLE = "CO2_ABCDaysPerCycle";
const std::string CO2VALUE_MINDAYSPERCYCLE = "CO2_ABCMinDaysPerCycle";
const std::string CO2VALUE_MAXDAYSPERCYCLE = "CO2_ABCMaxDaysPerCycle";
const std::string CO2VALUE_CALIBWAITTIME = "CO2_CalibWaitTime";



const std::string CO2VALUE_DEVICENAME = "CO2_DeviceName";
const std::string CO2VALUE_SERIALNO = "CO2_SerialNo";
const std::string CO2VALUE_SOFWAREVERSION = "CO2_SoftwareVersion";

const std::string CO2METHOD_GROUP = "CO2";
const std::string CO2METHOD_CALIBRATE = "CALIBRATE";
const std::string CO2METHOD_ENABLEABC = "ENABLEABC";
const std::string CO2METHOD_DISABLEABC = "DISABLEABC";
const std::string CO2METHOD_SETMAXPPM = "SETMAXPPM";



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
    Value _valMaxBasePPM { .i = 350 };
    Value _valMinBasePPM { .i = 2000 };
    Value _valBasePPM { .i = 400 };
    Value _valHasError { .b = false };
    Value _valIsHeatingUp { .b = false } ;
    Value _valIsAbcEnabled {.b = false };
    Value _valDaysPerAbcCycle;
    Value _valMinDaysPerAbcCycle { .i = 1 };
    Value _valMaxDaysPerAbcCycle { .i = 31};
    Value _valCalibWaitTime { .i = 2*60 };
public:
    CO2Sensor();

    virtual bool RefreshValues() = 0;
  


    virtual void DisableABC() = 0;
    virtual void ManualCalibration(int pBaseLinePPM) = 0;
    virtual void EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) = 0;
};

class ISensorManager {
public:
    virtual void EnableSensorReadGPIO() = 0;
    virtual void DisableSensorReadGPIO() = 0;
};