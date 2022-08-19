#pragma once

#include "Common.h"
#include "qrcode.h"
#include "DrawTarget.h"
#include <string>

class QRCodeGenerator {
public:
    QRCodeGenerator();
    
    void Draw(DrawTarget& pTarget, const Dimensions& pDimensions, const Position& pPosition, const std::string& pContent);
};