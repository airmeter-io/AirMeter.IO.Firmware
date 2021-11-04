#pragma once

#include "Common.h"
#include "qrcode.h"
#include "SsdDisplay.h"
#include <string>

class QRCodeGenerator {
public:
    QRCodeGenerator();
    
    void Display(SsdDisplay& pDisplay, const std::string& pContent);
};