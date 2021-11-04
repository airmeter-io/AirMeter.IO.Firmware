#pragma once

#include "Common.h"

class ButtonManager 
{
    static xQueueHandle _queue;
    static bool _isPressed;
    static TickType_t _pressedTick;
    static void gpio_isr_handler(ButtonManager *pManager);

public:
    ButtonManager();
    void ProcessQueue();
};