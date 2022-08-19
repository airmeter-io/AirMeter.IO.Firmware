#include "DrawTarget.h"


NullDrawTarget::NullDrawTarget() {

}

NullDrawTarget::~NullDrawTarget() {

}

void NullDrawTarget::BltMonoBitmap(uint8_t* pData, Dimensions pBitmapSize, Position pDrawAt, Rectangle pClipRectangle, DrawColor pColor) {

}

void NullDrawTarget::DrawFilledRectangle(int16_t pX, int16_t pY, int16_t pW, int16_t pH, DrawColor pColor) {

}

void NullDrawTarget::DrawPixel(int16_t x, int16_t y, DrawColor color) {

}

void NullDrawTarget::SetRotation(int16_t pRotation) {

}

void NullDrawTarget::DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, DrawColor color)  {

}

void NullDrawTarget::DrawDottedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, DrawColor color) {

}

void NullDrawTarget::Fill(DrawColor pColor) {

}

NullDrawControl::NullDrawControl() {
    _drawTarget = new NullDrawTarget();
}

NullDrawControl::~NullDrawControl()
{
    delete _drawTarget;
}

void NullDrawControl::RenderToDisplay(bool pFastUpdate) {
}

DrawTarget* NullDrawControl::GetDrawTarget() {
    return _drawTarget;
}