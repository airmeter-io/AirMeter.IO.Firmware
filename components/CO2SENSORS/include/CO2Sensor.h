#pragma once

#include<string>
#include<vector>



enum CO2SensorValueType { Bool, Int, Double, String};
class CO2SensorValue {
public:
    CO2SensorValueType type;
    union {
        bool b;
        int i;
        double d;
        std::string* s;
    } value;
    std::string name;
};

class CO2SensorAction {
    public:
        virtual std::string GetTitle() = 0;
        virtual bool GetIsDangerous()  = 0;
        virtual std::string& GetConfirmationTitle()  = 0;
        virtual std::string& GetConfirmationDescription()  = 0;
        virtual std::string& GetConfirmationButton()  = 0;
        virtual bool Invoke() = 0;
};


class CO2Sensor {
    std::vector<CO2SensorValue*> _values;
    std::vector<CO2SensorAction*> _actions;
protected:
    void RegisterValue(CO2SensorValue* pAdditionalValue);
    void RegisterAction(CO2SensorAction* pAction);
public:
    virtual std::string& GetSerialNo() = 0;    
    virtual std::string& GetDeviceName() = 0;
    virtual std::string& GetSWVersion() = 0;
    virtual bool RefreshValues() = 0;
    virtual int GetPPM() = 0;
    virtual int GetMaxPPM() = 0;
    virtual const std::vector<int>& GetAvailableMaxPPM() const = 0;
    virtual void SetMaxPPM(int pMaxPPM) = 0;    
    virtual bool GetIsHeatingUp() = 0;
    virtual bool GetHasError() = 0;
    virtual int GetBasePPM() = 0;
    virtual int GetDaysPerABCCycle() = 0;
    virtual int GetMinBasePPM();
    virtual int GetMaxBasePPM();
    virtual int GetMinDaysPerABCCycle();
    virtual int GetMaxDaysPerABCCycle();
    virtual bool GetIsABCEnabled() = 0;
    virtual void DisableABC() = 0;
    virtual void ManualCalibration(int pBaseLinePPM) = 0;
    virtual void EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) = 0;
    virtual int GetRequiredManualCalibrationWaitInSeconds();
    std::vector<CO2SensorValue*>& GetValues();
    std::vector<CO2SensorAction*>& GetActions();
};