#include "PowerLock.h"

PowerLock::PowerLock(PowerLockType pLockType, std::string pName) {
  auto espLockType = ESP_PM_CPU_FREQ_MAX; 
  switch(pLockType) {
    case MaxConfigured : 
      espLockType = ESP_PM_CPU_FREQ_MAX;
      break;
    case MaxPossible :
      espLockType = ESP_PM_APB_FREQ_MAX;
      break;
    case DisallowLightSleep : 
      espLockType = ESP_PM_NO_LIGHT_SLEEP;
      break;
  }    
  ESP_ERROR_CHECK(esp_pm_lock_create(espLockType, 0, pName.c_str(), &_handle ));
}

PowerLock::~PowerLock() {
    ESP_ERROR_CHECK(esp_pm_lock_delete(_handle));
}