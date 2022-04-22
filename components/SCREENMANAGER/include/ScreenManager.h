#pragma once

#include "Json.h"
#include "FontManager.h"
#include "ScreenDefinition.h"
#include "ButtonManager.h"
#include<vector>
class ScreenManager {
    FontManager _fontManager;
    std::vector<ScreenDefinition*> _screens;
    ScreenDefinition* _default = nullptr;
    ButtonManager* _buttons;
    void LoadScreens(Json& pJson);
    StringValueSource& _valueSource;
public:
    ScreenManager(StringValueSource& pValueSource);

    inline ScreenDefinition* GetDefault() {
        return _default;
    }

    void Run();
    void TriggerUpdate();
};