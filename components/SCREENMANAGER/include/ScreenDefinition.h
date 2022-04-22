#pragma once


#include "EPDDrawTarget.h"
#include "Json.h"
#include "FontManager.h"
#include "BitmapFont.h"
#include "StringWithValues.h"
#include<vector>
#include<string>

typedef struct {
    EPDDrawTarget& Target;
    StringValueSource& ValueSource;
} DrawContext;


class DrawAction {
public:
    virtual void Execute(DrawContext& pContext) = 0;
};

class DrawClearAction : public DrawAction {
    EPDColor _color;
public:
    DrawClearAction(Json& pJson);
    void Execute(DrawContext& pContext) override;
};

class DrawTextAction : public DrawAction {
    EPDColor _color;
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

class ScreenDefinition {
    std::vector<DrawAction*> _draw;
    std::string _name;
public:
    ScreenDefinition(const std::string pName);
    ~ScreenDefinition();

    void AddDraw(DrawAction* pAction);
    void ExecuteDraw(DrawContext& pContext);
};