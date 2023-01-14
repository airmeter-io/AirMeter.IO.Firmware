#include "CalibrationCommands.h"
#include "ValueController.h"
#include "CO2Sensor.h"

GetCalibrationInfoCommand::GetCalibrationInfoCommand()  {
    
}

void GetCalibrationInfoCommand::Process(Json& pJson,Json& pResult) {
    pResult.CreateStringProperty("Status", "true");
    for(auto keyPair : ValueController::GetCurrent().GetSourcesByName()) {
        auto source = keyPair.second->DefaultSource;
        if(source->GetFlags() & CALIBRATION_INFO ) {
            source->SerialiseToJsonProperty(pResult);
        }            
    }           
}

std::string GetCalibrationInfoCommand::GetName()  {
    return "GETLATESTCALIB";
}


ManualCalibrationCommand::ManualCalibrationCommand(){
    
}
void ManualCalibrationCommand::Process(Json& pJson,Json& pResult) {
    auto method = ValueController::GetCurrent().GetMethod(CO2METHOD_GROUP, CO2METHOD_CALIBRATE);
    if(!method) {
        pResult.CreateBoolProperty("Status", false);
        pResult.CreateStringProperty("Error", "CO2 Not enabled");                
        return;
    }
    method->Invoke({ {.i = 400}});
    pResult.CreateBoolProperty("Status", true);
    return;
}

std::string ManualCalibrationCommand::GetName() {
    return "MANUALCALIB";
}


EnableAbcCommand::EnableAbcCommand()  {
    
}

void EnableAbcCommand::Process(Json& pJson,Json& pResult)  {
    auto method = ValueController::GetCurrent().GetMethod(CO2METHOD_GROUP, CO2METHOD_ENABLEABC);
    if(!method) {
        pResult.CreateBoolProperty("Status", false);
        pResult.CreateStringProperty("Error", "CO2 Not enabled");                
        return;
    }
    if(pJson.HasProperty("Baseline") && pJson.HasProperty("ABCDaysPerCycle")) {
        auto baseLine = pJson.GetIntProperty("Baseline");
        auto daysPerCycle = pJson.GetIntProperty("ABCDaysPerCycle");
     
        method->Invoke({ {.i = baseLine}, {.i = daysPerCycle}});
        pResult.CreateBoolProperty("Status", true);
        return;
    }
    
    pResult.CreateBoolProperty("Status", false);
    pResult.CreateStringProperty("Error", "Missing Parameters");                   
}

std::string EnableAbcCommand::GetName() {
    return "ENABLEABC";
}

DisableAbcCommand::DisableAbcCommand() {
    
}

void DisableAbcCommand::Process(Json& pJson,Json& pResult) {
    auto method = ValueController::GetCurrent().GetMethod(CO2METHOD_GROUP, CO2METHOD_DISABLEABC);
    if(!method) {
        pResult.CreateBoolProperty("Status", false);
        pResult.CreateStringProperty("Error", "CO2 Not enabled");                
        return;
    }
    
    method->Invoke({});
    pResult.CreateBoolProperty("Status", true);            
}

std::string DisableAbcCommand::GetName()  {
    return "DISABLEABC";
}

