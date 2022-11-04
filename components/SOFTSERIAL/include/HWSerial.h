#pragma once
#include "Common.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
#include "PinSerial.h"
#include "DevicePersonality.h"
extern "C" {
    #include "driver/uart.h"
}

class HWSerial : public PinSerial {
    char _rxBuf[32];
    QueueHandle_t _uartQueue;
    UartConfig _uartConfig;
public:
    HWSerial(UartConfig pUartConfig);
    ~HWSerial();
    int Read(void *pBuf, int pLen) override;
    int Read(void *pBuf, int pLen, int pTimeout) override;
    bool Write(void *pBuf, int pLen) override;   
};
#endif
