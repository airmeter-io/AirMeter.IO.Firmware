
#include "Common.h"
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0)
#include "SoftSerial.h"

extern "C" {
    #include "softuart.h"
}

SoftSerial::SoftSerial(gpio_num_t pTxPin, gpio_num_t pRxPin) {
    softuart_open(0, 9600, (int)pRxPin, (int)pTxPin);
}

SoftSerial::~SoftSerial() {

}

int SoftSerial::Read(void *pBuf, int pLen) {
     char *cur = (char*)pBuf;
    for(int i = 0; i < pLen; i++, cur++) {
        while(!softuart_available(0)) taskYIELD();
        *cur = softuart_read(0);
    }
    return pLen;
}

bool SoftSerial::Write(void *pBuf, int pLen) {
    char *cur = (char*)pBuf;
    for(int i = 0; i < pLen; i++, cur++)
        if(!softuart_put(0, *cur)) return i;
    return pLen;
}

int SoftSerial::Read(void *pBuf, int pLen, int pTimeout) {
    auto start = xTaskGetTickCount();

    char *cur = (char*)pBuf;
    int i;
    for(i = 0; i < pLen &&  xTaskGetTickCount()  - start< pTimeout; i++, cur++) {
        while(!softuart_available(0) && xTaskGetTickCount()-start < pTimeout) {
         esp_task_wdt_reset();
        }
        if(softuart_available(0)) 
            *cur = softuart_read(0);
        else return i;
    }
    return i;
}
#endif