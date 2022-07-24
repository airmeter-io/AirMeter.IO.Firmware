#pragma once

#include "Common.h"
#include "qrcode.h"
#include "EPDDrawTarget.h"
#include <string>

class QRCodeGenerator {
public:
    QRCodeGenerator();
    
    void Draw(EPDDrawTarget& pTarget, const Dimensions& pDimensions, const Position& pPosition, const std::string& pContent);
};