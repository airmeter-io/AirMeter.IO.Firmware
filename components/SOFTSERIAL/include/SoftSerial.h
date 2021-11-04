#pragma once

#include "PinSerial.h"

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0)
class SoftSerial : public PinSerial {
    char _rxBuf[32];
    
public:
    PinSerial(gpio_num_t pTxPin, gpio_num_t pRxPin);
    ~PinSerial();
    int Read(void *pBuf, int pLen) override;
    int Read(void *pBuf, int pLen, int pTimeout) override;
    bool Write(void *pBuf, int pLen) override;   
};
#endif
