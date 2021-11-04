#pragma once

#include "Common.h"

#pragma once

class CaptiveDns {
    int _sockFd;
public:
    CaptiveDns();
    ~CaptiveDns();

    void ProcessRequest();
};