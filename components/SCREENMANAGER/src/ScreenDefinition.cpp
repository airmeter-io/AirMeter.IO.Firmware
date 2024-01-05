#include "ScreenDefinition.h"
#include "ScreenManager.h"
#include "QRCodeGenerator.h"
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
    for(auto draw : _draw){
        draw->Execute(pContext);
    }
}

void ScreenDefinition::ExecuteButton(DrawContext& pContext, ButtonEventCode pButtonEventCode) {
    if(!_buttonActions.count(pButtonEventCode)) {
        printf("No actions defined for button code %d\n",(int)pButtonEventCode);
        return;
    }

    
    for(auto action : _buttonActions[pButtonEventCode]){
        action->Execute(pContext);
    }
}


DrawClearAction::DrawClearAction(Json& pJson) {
    _color = DrawColor::White;
    if(pJson.HasProperty("Color")) {
        auto colorProp = pJson.GetStringProperty("Color");
        if(colorProp == "White")
            _color = DrawColor::White;
        if(colorProp == "Black")
            _color = DrawColor::Black;  
    } 
        
}

void DrawClearAction::Execute(DrawContext& pContext) {
    pContext.Target.Fill(_color);
}

DrawTextAction::DrawTextAction(Json& pJson, FontManager& pFontManager) {
    _color = DrawColor::White;
    if(pJson.HasProperty("Color")) {
        auto colorProp = pJson.GetStringProperty("Color");
        if(colorProp == "White")
            _color = DrawColor::White;
        if(colorProp == "Black")
            _color = DrawColor::Black;  
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
    _text.Generate(text);
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

DrawTimeSeriesAction::DrawTimeSeriesAction(Json& pJson, FontManager& pFontManager) {
    if(pJson.HasProperty("Value")) {
        _valueName = pJson.GetStringProperty("Value"); 
    } 

    _font = nullptr;

    if(pJson.HasProperty("Font")) {
        auto fontName = pJson.GetStringProperty("Font");
        _font = pFontManager.GetFont(fontName)->GetFont();
    }
    if(_font==nullptr) _font = pFontManager.GetDefaultFont()->GetFont();

    if(pJson.HasProperty("Color")) {
        auto colorProp = pJson.GetStringProperty("Color");
        if(colorProp == "White")
            _color = DrawColor::White;
        if(colorProp == "Black")
            _color = DrawColor::Black;  
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
    auto values = pContext.ValueSource.ResolveTimeSeries(_valueName, _mins*60, _steps);
    
    auto min = 400;
    auto max = 1000;

    
    for(auto val : values) {
        if(!val) continue;
        if(max<0 || max<val) max = val;
        if(min<0 || min>val) min = val;
    }

    auto range =(uint32_t)(max - min);
    if(range<=0)
        return;

    auto maxYText = std::to_string(max);
    auto minYText = std::to_string(min);
    auto midYText = std::to_string(min+(max-min)/2);
    auto minXText = std::string("-")+std::to_string(_mins)+"m";
    auto maxXText = std::string("Now");
    Dimensions maxYDim;
    Dimensions minYDim;
    Dimensions midYDim;
    Dimensions maxXDim;
    Dimensions minXDim;
    auto availableW = _w ;
    auto availableH = _h;
    auto perBar = availableW / _steps;
    auto offsetW = availableW - perBar*_steps;
    auto x = (int)(_x+offsetW);
    
    if(_font!=nullptr) {
        _font->MeasureUtf8(maxYText,250, maxYDim);
        _font->MeasureUtf8(minYText,250, minYDim);
        _font->MeasureUtf8(midYText,250, midYDim);
        _font->MeasureUtf8(minXText,250, minXDim);
        _font->MeasureUtf8(maxXText,250, maxXDim);
        
        auto maxYWidth = maxYDim.width > minYDim.width ? maxYDim.width : minYDim.width;
        auto maxXHeight = minXDim.height > maxXDim.height ? minXDim.height : maxXDim.height;
        if(maxYWidth<midYDim.width) maxYWidth = midYDim.width;
        maxYWidth+=3;
        maxXHeight+=3;
        availableW-= maxYWidth - 2;
        availableH-= maxXHeight - 2;
        x+= (maxYWidth + 2);
        perBar = availableW / _steps;
        _font->DrawUtf8(maxYText,pContext.Target, {.pos {. x = _x+(int)offsetW, .y = _y},.size { .width = maxYWidth, .height =maxYDim.height}},_color, DrawTextJustify::Right,DrawTextVerticalAlign::Top,2);
        _font->DrawUtf8(midYText,pContext.Target, {.pos {. x = _x+(int)offsetW, .y = _y+(availableH/2)-midYDim.height},.size { .width = maxYWidth, .height =midYDim.height}},_color, DrawTextJustify::Right,DrawTextVerticalAlign::Bottom,2);
        _font->DrawUtf8(minYText,pContext.Target, {.pos {. x = _x+(int)offsetW, .y = _y+availableH-minYDim.height},.size { .width = maxYWidth, .height =minYDim.height}},_color, DrawTextJustify::Right,DrawTextVerticalAlign::Bottom,2);
        _font->DrawUtf8(minXText,pContext.Target, {.pos {. x = x-minXDim.width/2, .y = _y+availableH-6},.size { .width = minXDim.width+5, .height =maxXHeight}},_color, DrawTextJustify::Center,DrawTextVerticalAlign::Bottom,2);
        _font->DrawUtf8(maxXText,pContext.Target, {.pos {. x = x+(int)perBar*(int)_steps-(int)maxXDim.width-3, .y = _y+availableH-6},.size { .width = maxXDim.width+5, .height =maxXHeight}},_color, DrawTextJustify::Right,DrawTextVerticalAlign::Bottom,2);
    } else 
        printf("Null Font\n");
    pContext.Target.DrawLine(x-2,_y, x, _y, _color);
    pContext.Target.DrawLine(x-2,_y+availableH/2, x, _y+availableH/2, _color);
    pContext.Target.DrawLine(x-2,_y+availableH, x, _y+availableH, _color);
    pContext.Target.DrawLine(x,_y, x, _y+availableH, _color);
    pContext.Target.DrawLine(x,_y+availableH, x+(int)perBar*(int)_steps, _y+availableH, _color);
    pContext.Target.DrawDottedLine(x,_y+availableH/2, x+(int)perBar*(int)_steps, _y+availableH/2, _color);
    auto totalGraphWidth = (int)perBar*(int)_steps;
    for(auto quarterHr = 0; quarterHr<= 4; quarterHr++) 
        pContext.Target.DrawLine( x+(totalGraphWidth*quarterHr)/4,_y+availableH, x+(totalGraphWidth*quarterHr)/4, _y+availableH+3, _color);
    for(auto fiveMins = 0; fiveMins< 12; fiveMins++) 
        if(fiveMins%3)
            pContext.Target.DrawLine( x+(totalGraphWidth*fiveMins)/12,_y+availableH, x+(totalGraphWidth*fiveMins)/12, _y+availableH+1, _color);
        
       
    for(auto step = 0; step < _steps && values.size()>step; step++) {

        if(!values[step]) continue;
        auto height = ((uint32_t )availableH* ((uint32_t)values[step]-min))/ range;

        pContext.Target.DrawFilledRectangle(x+step*perBar, availableH-height+_y,perBar,height, _color);
    }

}

DrawQRCodeAction::DrawQRCodeAction(Json& pJson) {
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
    if(pJson.HasProperty("Text"))
        _text = StringWithValues(pJson.GetStringProperty("Text"));
    else
        _text =StringWithValues(std::string(""));
}

void DrawQRCodeAction::Execute(DrawContext& pContext) {
    std::string text;
    _text.Generate(text);
    Rectangle rect =  { .pos = { .x = _x, .y =_y }, .size = {.width = _w, .height = _h } };
    QRCodeGenerator qrCode;
    qrCode.Draw(pContext.Target,rect.size, rect.pos, text);
}
