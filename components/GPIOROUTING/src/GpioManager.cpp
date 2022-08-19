#include "GpioManager.h"

#define TAG "GPIO"

std::vector<GpioGroup*> GpioManager::_groups;
std::map<gpio_num_t,GpioInternalState>GpioManager::_gpioMap;

void GpioManager::gpio_isr_handler(gpio_num_t pGpio) {
    auto level = gpio_get_level(pGpio);
    auto group = _gpioMap[pGpio];
    auto now = esp_timer_get_time();  

    
    if(level != group.Level) {
      //  ets_printf("GPIO fired %d, level %d\n",(int)pGpio, (int)level);
        GpioEvent event = { .Gpio = pGpio, .Level = level, .When = now };
        xQueueSendFromISR(group.Group->GetQueueHandle(), &event, NULL);
        _gpioMap[pGpio].Level = level;
    }

}

#define GPIO_INPUT_IO_0    (gpio_num_t)GPIO_NUM_13
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0))
void GpioManager::Setup() {    
    gpio_install_isr_service(0);
}
void GpioManager::RegisterGpioGroup(GpioGroup* pGroup) {
    _groups.push_back(pGroup);
    gpio_config_t io_conf;
    auto gpios = pGroup->GetGPIOs();
    for(auto gpio : gpios ) {
        GpioInternalState state = {.Gpio = gpio, .Group  = pGroup, .Level = 0 };
        _gpioMap[gpio] = state;
        printf("Mapping GPIO %d\n", (int)gpio);
        io_conf.pin_bit_mask =((1ULL<<gpio));
    }

    gpio_int_type_t intrType  = GPIO_INTR_ANYEDGE;
    switch(pGroup->GetInteruptType()) {
        case FallingEdge :
            intrType = GPIO_INTR_NEGEDGE;    
            printf("Configuring falling edge\n");
            break;
        case RaisingEdge : 
            intrType = GPIO_INTR_POSEDGE;    
            break;
        case AnyEdge : 
            intrType = GPIO_INTR_ANYEDGE;    
            break;
    }
    io_conf.intr_type = intrType;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = pGroup->GetPullUp() ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = pGroup->GetPullDown() ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);
    for(auto gpio : gpios ) {
        gpio_set_intr_type(gpio, intrType);

        gpio_isr_handler_add(gpio, (gpio_isr_t)gpio_isr_handler,(void *)gpio);
        printf("Setting level\n");
        switch(pGroup->GetInitialGpioState()) {
            case Low:                 
                gpio_set_level(gpio, 0);   
                printf("Set low\n");
                break;
            case High: 
                gpio_set_level(gpio, 1);
                printf("Set high\n");
                break;
            default:
                break;   
        }
        gpio_set_direction(gpio, GPIO_MODE_INPUT);
        gpio_set_pull_mode(gpio, pGroup->GetPullUp() &&  pGroup->GetPullDown() ? 
                                #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
                                    GPIO_PULLUP_PULLDOWN :
                                #else
                                    GPIO_PULLUP_ONLY :
                                #endif 
                                 pGroup->GetPullUp() ? GPIO_PULLUP_ONLY :
                                 pGroup->GetPullDown() ?GPIO_PULLDOWN_ONLY : 
                                 GPIO_FLOATING   );
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
        gpio_intr_enable(gpio);
#endif 

        
    }
     
}

GpioGroup::GpioGroup(std::vector<gpio_num_t>& pGpios, bool pPullUp, bool pPullDown, InteruptType pInteruptType, InitialGpioState pInitialGpioState) {
    _queue = xQueueCreate(10, sizeof(GpioEvent));
    _last = xTaskGetTickCount();
    _gpios = pGpios;
    _pullUp = pPullUp;
    _pullDown = pPullDown;
    _interuptType = pInteruptType;
    _initialGpioState = pInitialGpioState;
}

GpioGroup::~GpioGroup() {

}


void GpioGroup::WaitForEvents(TickType_t pTimeout) {
    GpioEvent event;
  
    xQueueReceive(_queue, &event,pTimeout); 

    _queuedEvents.push(event);
    _last = event.When;
    while(xQueueReceive(_queue, &event, 0)) {
        
        _queuedEvents.push(event);
        _last = event.When;        
    }
}

void GpioGroup::WaitForEvents() {
    WaitForEvents(portMAX_DELAY);    
}

void GpioGroup::PollEvents() {
    GpioEvent event;

    while(xQueueReceive(_queue, &event, 0)) {
        
        _queuedEvents.push(event);
        _last = event.When;        
    }
}

bool GpioGroup::HasQueued() {
    return _queuedEvents.size()>0;
}

GpioEvent GpioGroup::GetQueued() {
    auto result = _queuedEvents.front();
    _queuedEvents.pop();
    return result;
}

std::vector<gpio_num_t> GpioGroup::GetGPIOs() {
    return _gpios;
}

xQueueHandle GpioGroup::GetQueueHandle() const {
    return _queue;
}


bool GpioGroup::GetPullUp() const {
    return _pullUp;
}

bool GpioGroup::GetPullDown() const {
    return _pullDown;
}

InteruptType GpioGroup::GetInteruptType() const {
    return _interuptType;
}

InitialGpioState GpioGroup::GetInitialGpioState() const {
    return _initialGpioState;
}

void GpioGroup::Disable() {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    for(auto gpio : _gpios)
        gpio_intr_disable(gpio);
#endif
}

void GpioGroup::Enable() {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    for(auto gpio : _gpios)
        gpio_intr_enable(gpio);
#endif    
}





