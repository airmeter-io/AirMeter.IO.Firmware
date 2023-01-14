#pragma once

#include "GeneralSettings.h"

#include<string>

class CO2MonitorDisplay {
protected:
    GeneralSettings& _settings;
public:
    CO2MonitorDisplay(GeneralSettings& pSettings);

    virtual void RenderReadings() = 0;
    virtual void RenderAccessPointInfo(const std::string& pSSID, const std::string& pPassword) = 0;
    virtual bool IsPresent() = 0;
    virtual bool SupportsHalfOn() = 0;
    virtual void On(bool pHalf) = 0;
    virtual void Off() = 0;
};