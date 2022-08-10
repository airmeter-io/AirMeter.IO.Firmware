#pragma once
#include "Common.h"


extern "C" {
    #include "esp_adc_cal.h"
}

class BatteryManager {
    esp_adc_cal_characteristics_t _adcCharacteristics;
public:
    BatteryManager();   
    ~BatteryManager();   

    float GetBatteryVoltage();
};