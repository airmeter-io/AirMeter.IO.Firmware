#pragma once

#include "Common.h"
#include "SensorManager.h"
#include "MonitorDisplay.h"
#include "WifiTask.h"
#include "GeneralSettings.h"
#include "ButtonManager.h"
#include "BME280.h"
#include "StringWithValues.h"
#include "DataManagerStore.h"
#include "I2C.h"
#include "ScreenManager.h"
#include "MqttManager.h"
#include "BatteryManager.h"
#include "DevicePersonality.h"
#include "ScreenDrivers.h"
#include "ValueController.h"

class MainLogicLoop : private StringValueSource, private ScreenManagerNotifier,  public ValuesSource {
    std::string _timeString = "";
    std::string _version = "";
    std::string _build = "";
    Value _valUnixTime { .i = 0 };  
    Value _valTimeString { .s = &_timeString };    
    Value _valVersion { .s = &_version };    
    Value _valBuild { .s = &_build }; 
    Value _valBatteryVolts { .i = 0 };    
    SensorManager* _sensorManager = nullptr;
    GeneralSettings* _generalSettings = nullptr;
    DevicePersonality* _devicePersonality = nullptr;
    //CO2MonitorDisplay* _display = nullptr;
    MqttManager* _mqtt = nullptr;
    DataManagerStore *_dataManager = nullptr;
    ScreenManager *_screenManager = nullptr;
    BatteryManager *_battery = nullptr;
    WifiTask* _wifi = nullptr;
    ScreenDrivers  *_screenDrivers = nullptr;
    I2C* _i2c;
    std::vector<int> ResolveTimeSeries(std::string pName, uint32_t pSecondsInPast, uint32_t pSteps) override;
    bool ProcessOnUIThread() override;
    time_t _lastRead = 0;
    time_t _lastTimeInMinutes = 0;
    uint32_t _uiProcessCount = 0;
    bool _gotNtp = false;
    bool _waitingForProvisioning = false;


public:
    MainLogicLoop();
    const std::string& GetValuesSourceName() const override; 
    void Run();
};