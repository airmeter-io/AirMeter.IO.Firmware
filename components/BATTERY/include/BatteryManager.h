#pragma once
#include "Common.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
extern "C" {
    #include "esp_adc_cal.h"
}
#endif


class BatteryManager {
    esp_adc_cal_characteristics_t _adcCharacteristics;
public:
    BatteryManager();   
    ~BatteryManager();   

    float GetBatteryVoltage();
};