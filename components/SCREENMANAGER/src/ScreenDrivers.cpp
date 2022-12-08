#include "ScreenDrivers.h"
#include "DriverSSD16xx.h"


ScreenDrivers::ScreenDrivers() {
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0) 
    _drivers.push_back(new DriverSSD16xx());
    #endif
}

DrawControl *ScreenDrivers::LoadDriver(const std::string pName, Json* pConfiguration) {
    for(auto driver : _drivers) 
        if(driver->GetName() == pName)
            return driver->LoadDriver(pConfiguration);

    return new NullDrawControl();
}