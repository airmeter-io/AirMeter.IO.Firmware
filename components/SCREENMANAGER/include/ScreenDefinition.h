#pragma once


#include "DrawTarget.h"
#include "Json.h"
#include "FontManager.h"
#include "BitmapFont.h"
#include "StringWithValues.h"
#include "ButtonManager.h"
#include "SensorManager.h"
#include<vector>
#include<map>
#include<string>

class ScreenManager;

typedef struct {
    DrawTarget& Target;
    StringValueSource& ValueSource;
    SensorManager& Sensors;
    ScreenManager& Screens;
} DrawContext;

class DrawAction {
public:
    virtual void Execute(DrawContext& pContext) = 0;
};

class DrawClearAction : public DrawAction {
    DrawColor _color;
public:
    DrawClearAction(Json& pJson);
    void Execute(DrawContext& pContext) override;
};

class DrawTextAction : public DrawAction {
    DrawColor _color;
    BitmapFont* _font;
    int _x;
    int _y;
    int _w;
    int _h;
   
    StringWithValues _text;
    DrawTextJustify _justify;
    DrawTextVerticalAlign _verticalAlign;
public:
    DrawTextAction(Json& pJson, FontManager& pFontManager);
    void Execute(DrawContext& pContext) override;
};

class DrawTimeSeriesAction : public DrawAction {
    std::string _valueName;
    DrawColor _color;
    BitmapFont* _font;
    int _x;
    int _y;
    int _w;
    int _h;   
    uint32_t _mins;
    uint32_t _steps;  
public:
    DrawTimeSeriesAction(Json& pJson, FontManager& pFontManager);
    void Execute(DrawContext& pContext) override;
};

class DrawQRCodeAction : public DrawAction {
    StringWithValues _text;
    int _x;
    int _y;
    int _w;
    int _h;
public:
    DrawQRCodeAction(Json& pJson);
    void Execute(DrawContext& pContext) override;
};


class ChangeScreenAction : public DrawAction {
    std::string _screen;
public:
    ChangeScreenAction(Json& pJson);
    void Execute(DrawContext& pContext) override;
};

class TriggerUpdateAction : public DrawAction {
public:
    TriggerUpdateAction(Json& pJson);
    void Execute(DrawContext& pContext) override;
};

class CalibrateAction : public DrawAction {
public:
    CalibrateAction(Json& pJson);
    void Execute(DrawContext& pContext) override;
};

class ScreenDefinition {
    std::vector<DrawAction*> _draw;
    std::map<ButtonEventCode, std::vector<DrawAction*>> _buttonActions;
    std::string _name;
public:
    ScreenDefinition(const std::string pName);
    ~ScreenDefinition();

    void AddDraw(DrawAction* pAction);
    void AddButton(ButtonEventCode pButtonEventCode,std::vector<DrawAction*> &pActions);
    void ExecuteDraw(DrawContext& pContext);
    void ExecuteButton(DrawContext& pContext, ButtonEventCode pButtonEventCode);

    inline std::string GetName() {
        return _name;
    }
};

