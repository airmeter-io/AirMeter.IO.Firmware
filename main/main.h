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
#include "MqttManager.h"
#include "BatteryManager.h"
#include "DevicePersonality.h"

class MainLogicLoop : private StringValueSource, private ScreenManagerNotifier {
    
    SensorManager* _sensorManager = nullptr;
    GeneralSettings* _generalSettings = nullptr;
    DevicePersonality* _devicePersonality = nullptr;
    ValueModel _values;
    //CO2MonitorDisplay* _display = nullptr;
    MqttManager* _mqtt = nullptr;
    DataManagerRamTemporaryStore *_ramStore = nullptr;
    DataManagerFlashDataStore *_flashStore;
    DataManager *_dataManager = nullptr;
    ScreenManager *_screenManager = nullptr;
    BatteryManager *_battery = nullptr;
    Wifi* _wifi = nullptr;
    char _voltageStr[10];
    I2C* _i2c;
    std::string ResolveValue(std::string pName) override;
    std::vector<int> ResolveTimeSeries(std::string pName, uint32_t pSecondsInPast, uint32_t pSteps) override;
    bool ProcessOnUIThread() override;
    time_t _lastRead = 0;
    uint32_t _uiProcessCount = 0;
    bool _gotNtp = false;
    bool _waitingForProvisioning = false;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    EpdSpi _io;
#endif
public:
    MainLogicLoop();

    void Run();
};