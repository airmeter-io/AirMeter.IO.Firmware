#include "ScreenDefinition.h"
#include "ScreenManager.h"

ScreenDefinition::ScreenDefinition(const std::string pName) : _name(pName) {

}


ScreenDefinition::~ScreenDefinition() {

}

void ScreenDefinition::AddDraw(DrawAction* pAction) {
    _draw.push_back(pAction);
}

void ScreenDefinition::AddButton(ButtonEventCode pButtonEventCode,std::vector<DrawAction*> &pActions) {
    _buttonActions[pButtonEventCode] = pActions;
}

void ScreenDefinition::ExecuteDraw(DrawContext& pContext) {
 //   printf("Executing drae actions\n");
  //  printf("no draw %d\n",(int)_draw.size());
    for(auto draw : _draw){
    //    printf("1\n");
        draw->Execute(pContext);
       // printf("2\n");
    }

    //printf("Done executing\n");
}

void ScreenDefinition::ExecuteButton(DrawContext& pContext, ButtonEventCode pButtonEventCode) {
    if(!_buttonActions.count(pButtonEventCode)) {
        printf("No actions defined for button code %d\n",(int)pButtonEventCode);
        return;
    }

    printf("Executing button actions %d\n",(int)pButtonEventCode);
    for(auto action : _buttonActions[pButtonEventCode]){
        printf("Action\n");
        action->Execute(pContext);
    }
}


DrawClearAction::DrawClearAction(Json& pJson) {
    printf("Clear draw item\n");
    _color = EPDColor::White;
    if(pJson.HasProperty("Color")) {
        auto colorProp = pJson.GetStringProperty("Color");
        if(colorProp == "White")
            _color = EPDColor::White;
        if(colorProp == "Black")
            _color = EPDColor::Black;  
    } 
        
}

void DrawClearAction::Execute(DrawContext& pContext) {
    pContext.Target.GetBackBuffer().Fill(_color);
}

DrawTextAction::DrawTextAction(Json& pJson, FontManager& pFontManager) {
    _color = EPDColor::White;
    printf("Text draw item\n");
    if(pJson.HasProperty("Color")) {
        auto colorProp = pJson.GetStringProperty("Color");
        if(colorProp == "White")
            _color = EPDColor::White;
        if(colorProp == "Black")
            _color = EPDColor::Black;  
    } 
    _font = nullptr;

    if(pJson.HasProperty("Font")) {
        auto fontName = pJson.GetStringProperty("Font");
        _font = pFontManager.GetFont(fontName)->GetFont();
    }
    if(_font==nullptr) _font = pFontManager.GetDefaultFont()->GetFont();

    if(pJson.HasProperty("X") )
        _x = pJson.GetIntProperty("X");
    else
        _x  = 0;

    if(pJson.HasProperty("Y") )
        _y = pJson.GetIntProperty("Y");
    else
        _y  = 0;
        
    if(pJson.HasProperty("W") )
        _w = pJson.GetIntProperty("W");
    else
        _w  = 0;
        
    if(pJson.HasProperty("H") )
        _h = pJson.GetIntProperty("H");
    else
        _h  = 0;
        
    _justify = DrawTextJustify::Left;
    if(pJson.HasProperty("Justify")) {
        
        auto justify = pJson.GetStringProperty("Justify");
        printf("Screen has justify %s\n",justify.c_str());
        if(justify == "Left") _justify = DrawTextJustify::Left;
        else if(justify == "Right") _justify = DrawTextJustify::Right;
        else if(justify == "Center") _justify = DrawTextJustify::Center;
    }


    _verticalAlign = DrawTextVerticalAlign::Top;
    if(pJson.HasProperty("VerticalAlign")) {
        auto vertical = pJson.GetStringProperty("VerticalAlign");
        if(vertical == "Top") _verticalAlign = DrawTextVerticalAlign::Top;
        else if(vertical == "Middle") _verticalAlign = DrawTextVerticalAlign::Middle;
        else if(vertical == "Bottom") _verticalAlign = DrawTextVerticalAlign::Bottom;
    }

    if(pJson.HasProperty("Text"))
        _text = StringWithValues(pJson.GetStringProperty("Text"));
    else
        _text =StringWithValues(std::string(""));;
}

void DrawTextAction::Execute(DrawContext& pContext) {
    std::string text;
    _text.Generate(pContext.ValueSource, text);
    Rectangle rect =  { .pos = { .x = _x, .y =_y }, .size = {.width = _w, .height = _h } };
    _font->DrawUtf8(text, pContext.Target,rect, _color, _justify, _verticalAlign,2);
}

ChangeScreenAction::ChangeScreenAction(Json& pJson) {
    if(pJson.HasProperty("Screen")) {
        _screen = pJson.GetStringProperty("Screen"); 
    } 
}

void ChangeScreenAction::Execute(DrawContext& pContext) {
    pContext.Screens.ChangeScreen(pContext, _screen);
}

CalibrateAction::CalibrateAction(Json& pJson) {

}

void CalibrateAction::Execute(DrawContext& pContext) {
    pContext.Sensors.Calibrate();
}

TriggerUpdateAction::TriggerUpdateAction(Json& pJson) {

}

void TriggerUpdateAction::Execute(DrawContext& pContext) {
    pContext.Screens.TriggerUpdate(pContext);
}

DrawTimeSeriesAction::DrawTimeSeriesAction(Json& pJson) {
    if(pJson.HasProperty("Value")) {
        _valueName = pJson.GetStringProperty("Value"); 
    } 

    if(pJson.HasProperty("X") )
        _x = pJson.GetIntProperty("X");
    else
        _x  = 0;

    if(pJson.HasProperty("Y") )
        _y = pJson.GetIntProperty("Y");
    else
        _y  = 0;
        
    if(pJson.HasProperty("W") )
        _w = pJson.GetIntProperty("W");
    else
        _w  = 0;
        
    if(pJson.HasProperty("H") )
        _h = pJson.GetIntProperty("H");
    else
        _h  = 0;

    if(pJson.HasProperty("Steps") )
        _steps = (uint32_t)pJson.GetIntProperty("Steps");
    else
        _steps  = 0;
        
    if(pJson.HasProperty("Minutes") )
        _mins = (uint32_t)pJson.GetIntProperty("Minutes");
    else
        _mins  = 0;
}

void DrawTimeSeriesAction::Execute(DrawContext& pContext) {
    printf("Getting time series\n");
    auto values = pContext.ValueSource.ResolveTimeSeries(_valueName, _mins*60, _steps);
    printf("Got time series\n");
    
    auto min = -1;
    auto max = -1;
    
    for(auto val : values) {
        if(max<0 || max<val) max = val;
        if(min<0 || min>val) min = val;
    }
    auto range =(uint32_t)(max - min);
    if(range==0)
        return;

    auto perBar = _w / _steps;
    for(auto step = 0; step < _steps; step++) {
        printf("%d: %d\n", step, values[step]);
        if(!values[step]) continue;
        int32_t height = ((int32_t)_h  * (int32_t)values[step])/range;
        pContext.Target.DrawFilledRectangle(_x+step*perBar, _h-height+_y,perBar,height, EPDColor::Black);
    }

}