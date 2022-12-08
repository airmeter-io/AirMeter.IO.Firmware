#pragma once

#include "Json.h"
#include "DrawTarget.h"
#include "ScreenDriver.h"

#include<vector>
#include<string>



class ScreenDrivers {
    std::vector<ScreenDriver*> _drivers;
   
public:
    ScreenDrivers();

    DrawControl *LoadDriver(const std::string pName, Json* pConfiguration);
};