#pragma once


extern "C" {
    #include "esp_pm.h"
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
            return esp_pm_lock_acquire(_handle) == ESP_OK;
        }

        inline bool Release() {
            return esp_pm_lock_release(_handle) == ESP_OK;
        }
};