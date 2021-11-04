#pragma once

#include "Common.h"


class PinSerial  {
public:
    virtual int Read(void *pBuf, int pLen) = 0;
    virtual int Read(void *pBuf, int pLen, int pTimeout) = 0;
    virtual bool Write(void *pBuf, int pLen) = 0;  
};