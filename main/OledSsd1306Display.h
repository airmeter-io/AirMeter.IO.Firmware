#pragma once
#include "SsdDisplay.h"
#include "GeneralSettings.h"
#include "MonitorDisplay.h"
#include "ValueModel.h"
#include "I2C.h"
#include "Wifi.h"

class Oledssd1306Display : public CO2MonitorDisplay {
    enum LastRenderType {None, Readings, AccessPoint};
    LastRenderType _lastRenderType = None;

    char _lastPPM[10];
    char _lastTemp[10];
    char _lastHumdity[10];
    char _lastPressure[10];
    
    Wifi& _wifi;
    I2C& _i2c;

   // SsdDisplay *_ssd = nullptr;
public:
    Oledssd1306Display(GeneralSettings& pSettings, ValueModel& pValues, Wifi& pWifi, I2C& pI2C);

    void RenderReadings() override;
    void RenderAccessPointInfo(const std::string& pSSID, const std::string& pPassword) override;
    bool IsPresent() override;
    bool SupportsHalfOn() override;
    void On(bool pHalf) override;
    void Off() override;
};