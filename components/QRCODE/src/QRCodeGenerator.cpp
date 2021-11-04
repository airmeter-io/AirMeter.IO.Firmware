#include "QRCodeGenerator.h"

QRCodeGenerator::QRCodeGenerator() {
    
}

void QRCodeGenerator::Display(SsdDisplay& pDisplay, const std::string& pContent) {
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, 0, pContent.c_str());
    auto dotSize = pDisplay.GetHeight()/qrcode.size;
    uint8_t* xbm = (uint8_t*)malloc(pDisplay.GetHeight()*pDisplay.GetWidth()/8);
    memset(xbm, 0xff, pDisplay.GetHeight()*pDisplay.GetWidth()/8);
    auto xOffet = (pDisplay.GetWidth() - (qrcode.size*dotSize))/2;
    auto yOffet = (pDisplay.GetHeight() - (qrcode.size*dotSize))/2;
    pDisplay.FillRect(0,0, pDisplay.GetWidth(), pDisplay.GetHeight(), ssd1306_color_t::OLED_COLOR_WHITE);
    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            auto val = qrcode_getModule(&qrcode, x, y);
            pDisplay.FillRect(xOffet +x*dotSize, yOffet+y*dotSize, dotSize, dotSize, !val ?  ssd1306_color_t::OLED_COLOR_BLACK : ssd1306_color_t::OLED_COLOR_WHITE);
        }
    }
    pDisplay.Present();
}