#pragma once
#include<vector>
#include<string>
#include "Common.h"
#include "I2C.h"

class MB85FRAMManager {
    I2C& _i2c;
public:
    MB85FRAMManager(I2C& pI2C);
    
};