#pragma once


extern "C" {
    #include "esp_pm.h"
    #include "esp_err.h"
}

#include<string>

enum PowerLockType {
    MaxConfigured,
    MaxPossible,
    DisallowLightSleep
};

class PowerLock {
    esp_pm_lock_handle_t _handle;
    public:
        PowerLock(PowerLockType pType, std::string pName);
        ~PowerLock();
        inline bool Aquire() {
            auto ret = esp_pm_lock_acquire(_handle);
            ESP_ERROR_CHECK(ret);
            return ret == ESP_OK;
        }

        inline bool Release() {
            return esp_pm_lock_release(_handle) == ESP_OK;
        }
};