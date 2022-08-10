#pragma once

#include "Common.h"
#include<vector>
#include<queue>
#include<map>
#include "GpioManager.h"

enum  ButtonEventCode { Pressed = 0 , LongReleased, VeryLongReleased, Released };

typedef struct {    
    gpio_num_t Gpio;
    int64_t When;
    ButtonEventCode Code;
} ButtonEvent;


typedef struct {
    gpio_num_t Gpio;
    bool IsPressed;
    int64_t PressedTick;
} GpioState;

class ButtonManager {
    GpioGroup *_gpioGroup;
    GpioState* _gpioStates;
    std::queue<ButtonEvent> _queuedEvents;
    uint _gpioCount;
    bool IsPressed(gpio_num_t pGpio);
    void SetPressed(gpio_num_t pGpio, bool pPressed);
    int64_t GetPressedTick(gpio_num_t pGpio);
    void SetPressedTick(gpio_num_t pGpio, int64_t pPressed);

public:
    ButtonManager(std::vector<gpio_num_t>& pGpios);
    void WaitForEvents();
    void WaitForEvents(TickType_t pTimeout);
    void PollEvents();
    bool HasQueued();
    ButtonEvent GetQueued();
    inline GpioGroup* GetGpioGroup() { return _gpioGroup; }
};
