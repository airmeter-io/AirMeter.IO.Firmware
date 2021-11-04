#pragma once

#include "Common.h"

#include "BME280_driver/bme280.h"
#include "I2C.h"

class BME280SensorValues {
    char _pressureStr[20];
    char _temperatureStr[20];
    char _humidityStr[20];
public:
    double pressure  = 0;
    double temperature  = 0;
    double humidity = 0;

    char *GetPressureStr(int pDecimialDigits);
    char *GetTemperatureStr();
    char *GetHumidityStr();
};

class BME280 {
    I2C& _i2c;  
    struct bme280_dev _bme;  
    int _width;
    int _height;
    int _frameBufferSize;
    ssd1306_t _dev;
    uint8_t* _frameBuffer;
public:
    BME280(I2C& pI2C);
    ~BME280();

    I2C& GetI2C() const;
    uint8_t GetDeviceId() const;

    bool ReadSensorValues(BME280SensorValues &pReadValues);

};