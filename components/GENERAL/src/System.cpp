#include "System.h"

void System::Restart() {
    esp_restart();
}

char* System::GetMCUName(){
    esp_chip_info_t chipInfo;
    esp_chip_info(&chipInfo);
    switch((int)chipInfo.model) {
        case 0 : return (char*)"ESP8266";
        case (int)esp_chip_model_t::CHIP_ESP32 : return (char*)"ESP32";
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)        
        case (int)esp_chip_model_t::CHIP_ESP32S2 : return (char*)"ESP32S2";
        case (int)esp_chip_model_t::CHIP_ESP32S3 : return (char*)"ESP32S3";
        case (int)esp_chip_model_t::CHIP_ESP32C3 : return (char*)"ESP32C3";
#endif
    }
    return (char*)"Unknown";
}

int System::GetCoreCount() {
    esp_chip_info_t chipInfo;
    esp_chip_info(&chipInfo);
    return chipInfo.cores;
}


uint32_t System::GetFreeHeap() {
    return esp_get_free_heap_size();
}

uint32_t System::GetLeastHeapFreeSinceBoot() {
    return esp_get_minimum_free_heap_size();
}