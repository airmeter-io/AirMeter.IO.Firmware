#pragma once

#include "Common.h"
#include "SensorManager.h"
#include "MonitorDisplay.h"
#include "Wifi.h"
#include "GeneralSettings.h"
#include "ButtonManager.h"
#include "ValueModel.h"
#include "BME280.h"


class MainLogicLoop : private WifiUIImplementation {
    
    SensorManager* _sensorManager = nullptr;
    GeneralSettings* _generalSettings = nullptr;
    ValueModel _values;
    CO2MonitorDisplay* _display = nullptr;
    ButtonManager* _buttons;
    void ProcessEvents() override ;
    void DisplayAPAuthInfo(const std::string& pSSID, const std::string& pPassword) override;
public:
    void Run();
};