#include "PowerManagement.h"

extern "C" {
    #include "esp_pm.h"
    #include "esp_system.h"
    #include "esp_sleep.h"
}


void PowerManagement::Enable() {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    

#if CONFIG_IDF_TARGET_ESP32
    esp_pm_config_esp32_t pm_config = {
#elif CONFIG_IDF_TARGET_ESP32S2
    esp_pm_config_esp32s2_t pm_config = {
#elif CONFIG_IDF_TARGET_ESP32C3
    esp_pm_config_esp32c3_t pm_config = {
#elif CONFIG_IDF_TARGET_ESP32S3
    esp_pm_config_esp32s3_t pm_config = {
#endif
        .max_freq_mhz = 160,
        .min_freq_mhz = 40,
        .light_sleep_enable = true
    };
    printf("Min = %d, max = %d, light=%s\n",(int)pm_config.min_freq_mhz, (int)pm_config.max_freq_mhz, pm_config.light_sleep_enable ? "yes" : "no");
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
    esp_sleep_enable_gpio_wakeup();
#endif
}