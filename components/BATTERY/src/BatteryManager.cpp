#include "BatteryManager.h"

#define ADC_EN 14
extern "C" {    
    #include "driver/adc.h"
}
BatteryManager::BatteryManager() {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    gpio_set_direction((gpio_num_t)ADC_EN, GPIO_MODE_OUTPUT);
    for(auto i = 0; i < 8; i++)
        adc1_config_channel_atten((adc1_channel_t)i, ADC_ATTEN_DB_11);
    esp_adc_cal_characterize(
        (adc_unit_t)ADC_UNIT_1, 
        (adc_atten_t)ADC_ATTEN_DB_11, 
        (adc_bits_width_t)ADC_WIDTH_BIT_12, 
        1100, 
        &_adcCharacteristics);
#endif
}

BatteryManager::~BatteryManager() {

}

#define TAG "bat"

float BatteryManager::GetBatteryVoltage() {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    uint32_t reading;
  auto err = esp_adc_cal_get_voltage(
        (adc_channel_t)ADC1_CHANNEL_7,
        &_adcCharacteristics,
        &reading);
    ESP_LOGI(TAG, "BEFORE: %d mV",(int) reading);
   gpio_set_level((gpio_num_t)ADC_EN, 1);
     vTaskDelay(1 / portTICK_PERIOD_MS);
    err = esp_adc_cal_get_voltage(
        (adc_channel_t)ADC1_CHANNEL_7,
        &_adcCharacteristics,
        &reading);
    ESP_LOGI(TAG, "%d mV", (int)reading);
     gpio_set_level((gpio_num_t)ADC_EN, 0);
    if(err!=ESP_OK) printf("Error reading battery level: %d\n", (int)err);
    return (((float)reading)/4096)*7.26;
#else
    return 0;
#endif
}