#pragma once
#include "Common.h"
#include "ScreenDriver.h"


#include "EpdSpi.h"
#include "SSD1680.h"
#include "DrawControlSSD16xx.h"


class DriverSSD16xx : public ScreenDriver  {   
    EpdSpi* _io;
    SSD1680* _ssd1680;

    bool LoadPanel(std::string pName, SSD16xxConfiguration& pInfo);
    bool LoadPanelInfo(Json* pJson, SSD16xxConfiguration& pInfo);
    public:
        DrawControl* LoadDriver(Json* pConfiguration ) override;
        std::string GetName() const override ;     
};