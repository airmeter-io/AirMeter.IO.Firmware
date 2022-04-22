#include "ScreenDefinition.h"


ScreenDefinition::ScreenDefinition(const std::string pName) : _name(pName) {

}


ScreenDefinition::~ScreenDefinition() {

}

void ScreenDefinition::AddDraw(DrawAction* pAction) {
    _draw.push_back(pAction);
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


DrawClearAction::DrawClearAction(Json& pJson) {
  //  printf("Clear draw item\n");
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
  //  printf("Text draw item\n");
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
    _font->DrawUtf8(text, pContext.Target,rect, 0, _justify, _verticalAlign,2);
}