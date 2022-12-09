#include "PowerLock.h"

PowerLock::PowerLock(PowerLockType pLockType, std::string pName) {
    auto espLockType = (esp_pm_lock_type_t)pLockType;
    esp_pm_lock_create(espLockType, 0, pName.c_str(), &_handle );
}

PowerLock::~PowerLock() {
    esp_pm_lock_delete(_handle);
}