#pragma once

#include "Common.h"
#include "SensorManager.h"
#include "MonitorDisplay.h"
#include "Wifi.h"
#include "GeneralSettings.h"
#include "ButtonManager.h"
#include "ValueModel.h"
#include "BME280.h"
#include "StringWithValues.h"
#include "I2C.h"
#include "DataManagerRamTemporaryStore.h"
#include "DataManagerFlashDataStore.h"
#include "ScreenManager.h"


class MainLogicLoop : private WifiUIImplementation, private StringValueSource {
    
    SensorManager* _sensorManager = nullptr;
    GeneralSettings* _generalSettings = nullptr;
    ValueModel _values;
    CO2MonitorDisplay* _display = nullptr;

    DataManagerRamTemporaryStore *_ramStore = nullptr;
    DataManagerFlashDataStore *_flashStore;
    DataManager *_dataManager = nullptr;
    ScreenManager *_screenManager = nullptr;
    char _voltageStr[10];
    I2C* _i2c;
    time_t ProcessEvents() override ;
    void DisplayAPAuthInfo(const std::string& pSSID, const std::string& pPassword) override;
    std::string ResolveValue(std::string pName) override;
    std::vector<int> ResolveTimeSeries(std::string pName, uint32_t pSecondsInPast, uint32_t pSteps) override;
    time_t _lastRead = 0;
public:
    MainLogicLoop();
    void Run();
    void RunUI();
};