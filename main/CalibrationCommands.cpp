#include "CalibrationCommands.h"
#include "ValueController.h"
#include "CO2Sensor.h"

CalibrationCommand::CalibrationCommand(GeneralSettings& pSettings) : _settings(pSettings) {
    
}

void CalibrationCommand::Process(Json& pJson,Json& pResult) {    
    auto success = false;
    if(pJson.HasProperty("Mode")) {
        auto mode = pJson.GetStringProperty("Mode");

        if(mode == "Info") {
            for(auto groupPair : ValueController::GetCurrent().GetGroups()) {
                Json* groupJson = nullptr;
                for(auto keyPair : groupPair.second->SourcesByName) {
                    auto source = keyPair.second->DefaultSource;
                    if(source->GetFlags() & CALIBRATION_INFO ) {
                        if(groupJson == nullptr) 
                            groupJson = pResult.CreateObjectProperty(groupPair.first);
                        source->SerialiseToJsonProperty(*groupJson);
                    }            
                }   
                if(groupJson!=nullptr)
                    delete groupJson; 
            }    
            pResult.CreateNumberProperty("BackgroundCO2", _settings.GetBackgroundCO2() );    
            success = true;
        } else if(mode == "Perform") {
            auto method = ValueController::GetCurrent().GetMethod(GROUP_CO2, CO2METHOD_CALIBRATE);
            if(method) {               
                method->Invoke({ {.i = _settings.GetBackgroundCO2() }});
                success = true;
            }             
        } else if(mode == "SetOptions") {
            if(pJson.HasProperty("EnableABC") && pJson.HasProperty("ABCHoursPerCycle") && pJson.HasProperty("Baseline")) {
                auto enable = pJson.GetBoolProperty("EnableABC");
                auto cycleLenhth = pJson.GetIntProperty("ABCHoursPerCycle");
                auto baseLine = pJson.GetIntProperty("Baseline");
               
                if(enable) {
                    auto method = ValueController::GetCurrent().GetMethod(GROUP_CO2, CO2METHOD_ENABLEABC);
                    if(method!=nullptr) {
                        method->Invoke({ {.i = baseLine}, {.i = cycleLenhth}});
                        success = true;
                    }
                } else {
                    auto method = ValueController::GetCurrent().GetMethod(GROUP_CO2, CO2METHOD_DISABLEABC);
                    method->Invoke({});
                    success = true;
                }
                if(success) {
                    _settings.SetBackgroundCO2(baseLine);
                    _settings.Save();       
                }                
            } 
        } 
    }
    pResult.CreateBoolProperty("Status", success);    
}

std::string CalibrationCommand::GetName()  {
    return "CALIBRATE";
}


