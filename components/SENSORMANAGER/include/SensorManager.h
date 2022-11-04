#pragma once

#include "Common.h"
#include "CO2Sensor.h"
#include "ValueModel.h"
#include "BME280.h"
#include "GeneralSettings.h"
#include "DevicePersonality.h"
#include "I2C.h"
#include "PinSerial.h"
#include "DataManager.h"

class SensorManager : private ISensorManager {
    CO2Sensor* _co2Sensor = nullptr;
    PinSerial* _serial = nullptr;
    BME280* _bme;
    ValueModel _values;
    GeneralSettings& _settings;
    DevicePersonality& _devicePersonality;
    I2C& _i2c;    
    DataManager _dataManager;
    
    time_t _lastSensorRead = 0;
    void EnableSensorReadGPIO() override;
    void DisableSensorReadGPIO() override;
public:
    SensorManager(DevicePersonality& pDevicePersonality,GeneralSettings& pSettings, I2C& pI2C, DataManager& pDataManager);
    ~SensorManager();
    ValueModel& GetValues();
    time_t UpdateValues();
    void Calibrate();
    inline time_t GetLastSensorRead() { return _lastSensorRead; }
};