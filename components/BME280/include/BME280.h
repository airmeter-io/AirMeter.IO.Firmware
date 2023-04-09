#pragma once

#include "Common.h"

#include "BME280_driver/bme280.h"
#include "I2C.h"
#include "ValueController.h"


class BME280 : public ValuesSource {
    I2C& _i2c;  
    struct bme280_dev _bme;  
    int _width;
    int _height;
    int _frameBufferSize;
    uint8_t* _frameBuffer;

    Value _valPressure = { .i = 0 };
    Value _valTemperature = { .i = 0 };
    Value _valHumidity = { .i = 0 };
    const std::string SOURCE_NAME = "BME280";
protected: 
    const std::string& GetValuesSourceName() const override;
public:
    BME280(I2C& pI2C);
    ~BME280();

    I2C& GetI2C() const;
    uint8_t GetDeviceId() const;

    bool ReadSensorValues();

};