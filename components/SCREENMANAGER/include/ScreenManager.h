#pragma once

#include "Json.h"
#include "FontManager.h"
#include "ScreenDefinition.h"
#include "ButtonManager.h"
#include "SensorManager.h"
#include "DrawTarget.h"
#include "DevicePersonality.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
#include "DriverSSD1680.h"
#endif

#include<vector>
#include<string>

class ScreenManagerNotifier {
    public:
        virtual bool ProcessOnUIThread() = 0;
        
};

class ScreenManager {
    FontManager _fontManager;
    ScreenManagerNotifier& _notifier;
    std::vector<ScreenDefinition*> _screens;
    ScreenDefinition* _default = nullptr;
    ButtonManager* _buttons;
    ScreenDefinition* _current;
    DevicePersonality& _devicePersonality;
    StringValueSource& _valueSource;
    SensorManager& _sensorManager;
    DrawControl* _drawControl;

    std::string _nextScreen = "";
    bool _needScreenChange = false;
    bool _running = false;

    std::vector<DrawAction *> LoadActions(Json* pParentElement, std::string pElementName);
    void LoadScreens(Json& pJson);    
public:
    ScreenManager(DevicePersonality& pDevicePersonality,DrawControl* pDrawControl, StringValueSource& pValueSource, SensorManager& pSensorManager, ScreenManagerNotifier& pNotifier);

    inline ScreenDefinition* GetDefault() {
        return _default;
    }
    inline ScreenDefinition* GetCurrent() {
        return _current;
    }
    void Run(TickType_t pNotifyPeriod);
    void ChangeScreen(std::string pScreen);
    void ChangeScreen(DrawContext& pContext, std::string pScreen);
    void TriggerUpdate(DrawContext& pContext);
};