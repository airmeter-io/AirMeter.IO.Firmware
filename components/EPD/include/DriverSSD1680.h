#pragma once

#include "Common.h"
#include "EPDBackBuffer.h"
#include "SSD1680.h"
#include "DrawTarget.h"
#include "EPDDrawTarget.h"
#include "BitmapFont.h"
#include<string>
class DriverSSD1680 : public DrawControl {
    const uint WIDTH = 128;
    const uint HEIGHT = 250;
    EPDBackBuffer* _backBuffer = new EPDBackBuffer(WIDTH, HEIGHT);
    SSD1680 &_ssd1680;
    static SSD1680Lut LUTPartialUpdate;
    EPDDrawTarget *_gfx;
   // BitmapFont _font;
public:
    DriverSSD1680(SSD1680& pSsd1680);
    ~DriverSSD1680();

    inline EPDBackBuffer& GetBackBuffer() {
        return *_backBuffer;
    }

    void RenderToDisplay(bool pFastUpdate) override;
    DrawTarget* GetDrawTarget() override;
};