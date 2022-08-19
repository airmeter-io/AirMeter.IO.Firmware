#include "QRCodeGenerator.h"

QRCodeGenerator::QRCodeGenerator() {
    
}

void QRCodeGenerator::Draw(DrawTarget& pTarget, const Dimensions& pDimensions, const Position& pPosition, const std::string& pContent) {
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, 0, pContent.c_str());
    auto dotSize = pDimensions.height/qrcode.size;
   
    auto xOffet =pPosition.x+ (pDimensions.width - (qrcode.size*dotSize))/2;
    auto yOffet =pPosition.y+ (pDimensions.height - (qrcode.size*dotSize))/2;
    pTarget.DrawFilledRectangle(pPosition.x,pPosition.y, pDimensions.width,pDimensions.height,DrawColor::White);
    
    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            auto val = qrcode_getModule(&qrcode, x, y);
            pTarget.DrawFilledRectangle(xOffet +x*dotSize, yOffet+y*dotSize, dotSize, dotSize, val ?  DrawColor::Black : DrawColor::White);
         
        }
    }
}