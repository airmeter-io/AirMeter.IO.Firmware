#pragma once
#include<vector>
#include<string>
#include "Common.h"
#include "I2CDeviceSession.h"

class I2CDeviceSession;

class I2C {
    gpio_num_t _sdaPin;
    gpio_num_t _sdlPin;
    uint8_t* _devices = nullptr;    
    size_t _numDevices;
public:
    I2C(gpio_num_t pSdaPin, gpio_num_t pSclPin);
    bool Scan();
    size_t GetNumDevices();
    uint8_t GetDevice(uint pIndex);
    i2c_port_t GetI2CPort();
    I2CDeviceSession* CreateSession(uint8_t pDeviceId);
    ~I2C();
};