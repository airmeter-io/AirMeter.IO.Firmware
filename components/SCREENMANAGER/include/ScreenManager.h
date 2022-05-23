#pragma once

#include "Json.h"
#include "FontManager.h"
#include "ScreenDefinition.h"
#include "ButtonManager.h"
#include "SensorManager.h"
#include "DEPG0213BN.h"
#include<vector>
#include<string>
class ScreenManager {
    FontManager _fontManager;
    std::vector<ScreenDefinition*> _screens;
    ScreenDefinition* _default = nullptr;
    ButtonManager* _buttons;
    ScreenDefinition* _current;
    std::vector<DrawAction *> LoadActions(Json* pParentElement, std::string pElementName);
    void LoadScreens(Json& pJson);
    StringValueSource& _valueSource;
    SensorManager& _sensorManager;
    DEPG0213BN* _display = nullptr;
public:
    ScreenManager(StringValueSource& pValueSource, SensorManager& pSensorManager);

    inline ScreenDefinition* GetDefault() {
        return _default;
    }
    inline ScreenDefinition* GetCurrent() {
        return _current;
    }
    void Run();
    void TriggerUpdate();

    void ChangeScreen(DrawContext& pContext, std::string pScreen);
    void TriggerUpdate(DrawContext& pContext);
};