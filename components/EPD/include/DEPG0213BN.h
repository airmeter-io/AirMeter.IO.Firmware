#pragma once

#include "Common.h"
#include "EPDBackBuffer.h"
#include "SSD1680.h"
#include "EPDDrawTarget.h"
#include "BitmapFont.h"
#include<string>
class DEPG0213BN {
    const uint WIDTH = 128;
    const uint HEIGHT = 250;
    EPDBackBuffer* _backBuffer = new EPDBackBuffer(WIDTH, HEIGHT);
    SSD1680 &_ssd1680;
    static SSD1680Lut LUTPartialUpdate;
    EPDDrawTarget *_gfx;
   // BitmapFont _font;
public:
    DEPG0213BN(SSD1680& pSsd1680);
    ~DEPG0213BN();

    inline EPDBackBuffer& GetBackBuffer() {
        return *_backBuffer;
    }

    void UpdateFull();
    void UpdatePartial();
};