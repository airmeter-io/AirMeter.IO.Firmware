#pragma once

#include "Common.h"
#include "EPDBackBuffer.h"
#include "SSD1680.h"
#include "DrawTarget.h"
#include "EPDDrawTarget.h"
#include "BitmapFont.h"
#include<string>



class SSD16xxConfiguration {
public:
    uint16_t Width;
    uint16_t Height;
    int16_t RamXOffset;
    bool EnablePingPong;
    SSD1680Lut LUTPartialUpdate;    
};

class DrawControlSSD16xx : public DrawControl {
    EPDBackBuffer* _backBuffer;
    SSD1680 &_ssd1680;
    static SSD1680Lut LUTPartialUpdate;
    EPDDrawTarget *_gfx;
    const SSD16xxConfiguration _config;
   // BitmapFont _font;
public:
    DrawControlSSD16xx(SSD1680& pSsd1680, const SSD16xxConfiguration& pConfig);
    ~DrawControlSSD16xx();

    inline EPDBackBuffer& GetBackBuffer() {
        return *_backBuffer;
    }

    void RenderToDisplay(bool pFastUpdate) override;
    DrawTarget* GetDrawTarget() override;
};