#include "CalibrationCommands.h"



GetCalibrationInfoCommand::GetCalibrationInfoCommand(ValueModel& pValues) : _values(pValues) {
    
}

void GetCalibrationInfoCommand::Process(Json& pJson,Json& pResult) {
    pResult.CreateStringProperty("Status", "true");
    
    if(_values.CO2) {
        pResult.CreateBoolProperty(  "ABCEnabled",         _values.CO2->GetIsABCEnabled());
        pResult.CreateNumberProperty("MinBaseline",        _values.CO2->GetMinBasePPM());
        pResult.CreateNumberProperty("MaxBaseline",        _values.CO2->GetMaxBasePPM());
        pResult.CreateNumberProperty("Baseline",           _values.CO2->GetBasePPM());
        pResult.CreateNumberProperty("CalibWaitTime",      _values.CO2->GetRequiredManualCalibrationWaitInSeconds());
        pResult.CreateNumberProperty("MinABCDaysPerCycle", _values.CO2->GetMinDaysPerABCCycle());
        pResult.CreateNumberProperty("MaxABCDaysPerCycle", _values.CO2->GetMaxDaysPerABCCycle());
        pResult.CreateNumberProperty("DaysPerABCCycle",    _values.CO2->GetDaysPerABCCycle());                    
    }            
}

std::string GetCalibrationInfoCommand::GetName()  {
    return "GETLATESTCALIB";
}


ManualCalibrationCommand::ManualCalibrationCommand(ValueModel& pValues) : _values(pValues) {
    
}
void ManualCalibrationCommand::Process(Json& pJson,Json& pResult) {
    if(!_values.CO2) {
        pResult.CreateBoolProperty("Status", false);
        pResult.CreateStringProperty("Error", "CO2 Not enabled");                
        return;
    }
    _values.CO2->ManualCalibration(400);
    pResult.CreateBoolProperty("Status", true);
    return;
}
std::string ManualCalibrationCommand::GetName() {
    return "MANUALCALIB";
}


EnableAbcCommand::EnableAbcCommand(ValueModel& pValues) : _values(pValues) {
    
}

void EnableAbcCommand::Process(Json& pJson,Json& pResult)  {
    if(!_values.CO2) {
        pResult.CreateBoolProperty("Status", false);
        pResult.CreateStringProperty("Error", "CO2 Not enabled");                
        return;
    }
    
    if(pJson.HasProperty("Baseline") && pJson.HasProperty("ABCDaysPerCycle")) {
            auto baseLine = pJson.GetIntProperty("Baseline");
            auto daysPerCycle = pJson.GetIntProperty("ABCDaysPerCycle");
            _values.CO2->EnableABC(baseLine, daysPerCycle);
            pResult.CreateBoolProperty("Status", true);
            return;
    }
    
    pResult.CreateBoolProperty("Status", false);
    pResult.CreateStringProperty("Error", "Missing Parameters");                   
}

std::string EnableAbcCommand::GetName() {
    return "ENABLEABC";
}

DisableAbcCommand::DisableAbcCommand(ValueModel& pValues) : _values(pValues) {
    
}

void DisableAbcCommand::Process(Json& pJson,Json& pResult) {
    if(!_values.CO2) {
        pResult.CreateBoolProperty("Status", false);
        pResult.CreateStringProperty("Error", "CO2 Not enabled");                
        return;
    }
    
    _values.CO2->DisableABC();
    pResult.CreateBoolProperty("Status", true);            
}

std::string DisableAbcCommand::GetName()  {
    return "DISABLEABC";
}

