#include "ButtonManager.h"

#define TAG "BUTTONS"



ButtonManager::ButtonManager(std::vector<gpio_num_t>& pGpios) {
    _gpioGroup = new GpioGroup(pGpios, false, false, AnyEdge, NotSet);
    GpioManager::RegisterGpioGroup(_gpioGroup);
    _gpioCount = pGpios.size();
    _gpioStates =  (GpioState*)calloc(pGpios.size(), sizeof(GpioState));

    auto curGpio = _gpioStates;
    for(auto gpio : pGpios) {
        *curGpio= { .Gpio = gpio, .IsPressed = false, .PressedTick = 0 };
        curGpio++;
    }
}

bool ButtonManager::IsPressed(gpio_num_t pGpio) {
    for(auto i = 0; i<_gpioCount;i++)
        if(_gpioStates[i].Gpio == pGpio)
            return _gpioStates[i].IsPressed;
    return false;
}

void ButtonManager::SetPressed(gpio_num_t pGpio, bool pPressed) {
    for(auto i = 0; i<_gpioCount;i++)
        if(_gpioStates[i].Gpio == pGpio)
            _gpioStates[i].IsPressed = pPressed;
}

int64_t ButtonManager::GetPressedTick(gpio_num_t pGpio) {
    for(auto i = 0; i<_gpioCount;i++)
        if(_gpioStates[i].Gpio == pGpio)
            return _gpioStates[i].PressedTick;
    return 0;
}

void ButtonManager::SetPressedTick(gpio_num_t pGpio, int64_t pPressed) {
    for(auto i = 0; i<_gpioCount;i++)
        if(_gpioStates[i].Gpio == pGpio)
            _gpioStates[i].PressedTick = pPressed;
}


void ButtonManager::WaitForEvents() {
    bool hasTriggered = false;

    while(!hasTriggered) {
        _gpioGroup->WaitForEvents();
        while(_gpioGroup->HasQueued()) {
            auto event = _gpioGroup->GetQueued();
            if(event.Gpio == 0)
            {
                ButtonEvent btnEvent = {
                    .Gpio = event.Gpio,
                    .When = event.When,
                    .Code = ButtonEventCode::Pressed
                };
                _queuedEvents.push(btnEvent);   
                hasTriggered = true;      
                continue;
            }
            auto duration = event.When - GetPressedTick(event.Gpio);
            auto wasPressed = IsPressed(event.Gpio);
            if( duration > 1000 && (event.Level!=0)!=wasPressed) {
            
                ButtonEvent btnEvent = {
                    .Gpio = event.Gpio,
                    .When = event.When,
                    .Code = ButtonEventCode::Pressed
                };
                printf("Button Level Changed %d, %d\n", (int)event.Gpio,(int)event.Level);
                if(event.Level) 
                    SetPressed(event.Gpio, true);
                else {
                    SetPressed(event.Gpio, false);
                    if(duration < 500000) 
                        btnEvent.Code = ButtonEventCode::Released;
                    else if (duration < 1000000) 
                        btnEvent.Code = ButtonEventCode::LongReleased;
                    else 
                        btnEvent.Code = ButtonEventCode::VeryLongReleased;                
                }
                SetPressedTick(event.Gpio, event.When); 
                _queuedEvents.push(btnEvent);           
                hasTriggered = true;           
            } else 
                printf("Dropped event gpio = %d, duration = %d, level = %d\n", (int)event.Gpio, (int)duration, (int)event.Level);
        }    
    }
}

bool ButtonManager::HasQueued() {
    return _queuedEvents.size()>0;
}

ButtonEvent ButtonManager::GetQueued() {
    auto result = _queuedEvents.front();
    _queuedEvents.pop();
    return result;
}