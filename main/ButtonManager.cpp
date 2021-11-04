#include "ButtonManager.h"

#define TAG "BUTTONS"

enum  ButtonEventCode { Pressed = 0 , LongReleased, VeryLongReleased, Released };

xQueueHandle ButtonManager::_queue = xQueueCreate(10, sizeof(uint8_t));
bool ButtonManager::_isPressed = false;
TickType_t ButtonManager::_pressedTick = 0;
void ButtonManager::gpio_isr_handler(ButtonManager *pManager) {
    uint8_t code = 0;
    if(_isPressed) {
        auto duration = xTaskGetTickCount() - _pressedTick;
        _isPressed = false;
        if(duration < 100) {
            code = (uint8_t)ButtonEventCode::Released;
        } else if (duration < 300) {
            code = (uint8_t)ButtonEventCode::LongReleased;
        } else {
            code = (uint8_t)ButtonEventCode::VeryLongReleased;
        }
    } else {
        _isPressed = true;
        _pressedTick = xTaskGetTickCount();
        code = (uint8_t)ButtonEventCode::Pressed;
    }
    xQueueSendFromISR(_queue, &code, NULL);
}

#define GPIO_INPUT_IO_0    (gpio_num_t)0
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0))

ButtonManager::ButtonManager() {
    gpio_config_t io_conf;
    gpio_install_isr_service(0);
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = (gpio_pullup_t)1;
    gpio_config(&io_conf);
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);

    gpio_isr_handler_add(GPIO_INPUT_IO_0, (gpio_isr_t)gpio_isr_handler, (void *) GPIO_INPUT_IO_0);
    gpio_set_level(GPIO_INPUT_IO_0, 0);    
}

void ButtonManager::ProcessQueue() {
    uint8_t eventByte;
    ButtonEventCode eventCode;
    while (xQueueReceive(_queue, &eventByte, 0)) {
        eventCode = (ButtonEventCode)eventByte;;
        switch(eventCode) {
            case ButtonEventCode::Pressed:
                printf("Presssed\n");
                break;
            case ButtonEventCode::LongReleased:
                printf("Long Press\n");
                break;
            case ButtonEventCode::VeryLongReleased:
                printf("Very Long Press\n");
                break;                
            case ButtonEventCode::Released:
                printf("Released\n");
                break;
        }
       
    }
}