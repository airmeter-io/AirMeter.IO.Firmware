#pragma once

#include "Common.h"
#include<vector>
#include<queue>
#include<map>

typedef struct {
    gpio_num_t Gpio;
    int Level;
    int64_t When;
} GpioEvent;

class GpioGroup;

typedef struct {
    gpio_num_t Gpio;
    GpioGroup* Group;
    int Level;
} GpioInternalState;

enum InteruptType { AnyEdge, RaisingEdge, FallingEdge };
enum InitialGpioState { Low, High, NotSet };

class GpioGroup {
    xQueueHandle _queue;
    int64_t _last = 0;
    std::queue<GpioEvent> _queuedEvents;
    std::vector<gpio_num_t> _gpios;
    bool _pullUp;
    bool _pullDown;
    InteruptType _interuptType;
    InitialGpioState _initialGpioState;
public:
    GpioGroup(std::vector<gpio_num_t>& pGpios, bool pPullUp = true, bool pPullDown = false, InteruptType pInteruptType = AnyEdge, InitialGpioState pInitialGpioState = Low);
    ~GpioGroup();
    void WaitForEvents();
    void WaitForEvents(TickType_t pTimeout);
    void PollEvents();
    bool HasQueued();
    bool GetPullUp() const;
    bool GetPullDown() const;
    void Disable();
    void Enable();
    InitialGpioState GetInitialGpioState() const;
    InteruptType GetInteruptType() const;
    GpioEvent GetQueued();
    std::vector<gpio_num_t> GetGPIOs();
    xQueueHandle GetQueueHandle() const;    
};

class GpioManager 
{
    static std::vector<GpioGroup*> _groups;
    static std::map<gpio_num_t, GpioInternalState> _gpioMap;
    static void gpio_isr_handler(gpio_num_t pGpio);

public:
    static void Setup();
    static void RegisterGpioGroup(GpioGroup* pGroup);  
};

