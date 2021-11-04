#pragma once

#include "Common.h"
#include <string>

class PasswordGenerator {
public:
    PasswordGenerator();
    
    std::string Generate(int pLength);
};