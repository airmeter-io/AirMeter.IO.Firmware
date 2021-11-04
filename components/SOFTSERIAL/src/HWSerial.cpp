#include "Common.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
#include "HWSerial.h"
HWSerial::HWSerial(uart_port_t pUartNum) : _uartPort(pUartNum) {

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 2,
        .use_ref_tick = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_param_config(pUartNum, &uart_config));
    const int uartBufferSize = (1024 * 2);
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, 17, 16, -1, -1));
    ESP_ERROR_CHECK(uart_driver_install(pUartNum, uartBufferSize, uartBufferSize, 10, &_uartQueue, 0));
    printf("Serial initialized\n");
}

HWSerial::~HWSerial() {

}

int HWSerial::Read(void *pBuf, int pLen) {
    printf("Serial reading\n");
    auto result = uart_read_bytes(_uartPort, pBuf, (uint32_t)pLen, 0);
    printf("serial read %d bytes\n", result);
    return result;
}

bool HWSerial::Write(void *pBuf, int pLen) {
    return uart_write_bytes(_uartPort, pBuf, pLen)!=-1;    
}

int HWSerial::Read(void *pBuf, int pLen, int pTimeout) {
    printf("Serial reading (t)\n");
    auto result = uart_read_bytes(_uartPort, pBuf, (uint32_t)pLen, (TickType_t)pTimeout);
    printf("serial read %d bytes (t)\n", result);
    return result;
}

#endif
