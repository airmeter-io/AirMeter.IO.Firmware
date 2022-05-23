
#include "EPDDrawTarget.h"
EPDDrawTarget::EPDDrawTarget(EPDBackBuffer& pBuffer) : _buffer(pBuffer) {
  _rotation  = 0;
}

EPDDrawTarget::~EPDDrawTarget() {

}

void EPDDrawTarget::setRotation(int16_t pRotation) {
   _rotation = pRotation;
}



void EPDDrawTarget::drawPixel(int16_t x, int16_t y, EPDColor color) {
  AdjustCoOrdinatesForRotation(x,y);
  if (IsXOutOfRange(x) || IsYOutOfRange(y)) return;
  
  DrawPixelUnchecked(x,y,color);
}

void EPDDrawTarget::BltMonoBitmap(uint8_t* pData, Dimensions pBitmapSize, Position pDrawAt, Rectangle pClipRectangle, EPDColor pColor){
  auto dataOFfset = 0;
  uint8_t bits = 0;
  auto bit = 0;
  for (auto yy = 0; yy < pBitmapSize.height; yy++) {
      auto yInRange = pDrawAt.y+yy >=pClipRectangle.pos.y && pDrawAt.y+yy < pClipRectangle.pos.y+pClipRectangle.size.height;

      for (auto xx = 0; xx < pBitmapSize.width; xx++) {
        if (!(bit++ & 7)) {
          bits = pData[dataOFfset++];
        }

        if(pDrawAt.x+xx >=pClipRectangle.pos.x && pDrawAt.x+xx < pClipRectangle.pos.x+pClipRectangle.size.width && yInRange && bits & 0x80) {
          drawPixel(pDrawAt.x+ xx, pDrawAt.y + yy+6, pColor);    
        } 
        
        bits <<= 1;
      }
  }
}

void EPDDrawTarget::DrawFilledRectangle(int16_t pX, int16_t pY, int16_t pW, int16_t pH, EPDColor pColor) {
  printf("Draw Rect %d,%d (%d, %d)\n", (int)pX, (int)pY, (int)pW, (int)pH);
  AdjustCoOrdinatesForRotation(pX,pY);
  switch (_rotation)
  {
    case 1:
    case 3:
      Swap(pW, pH);
    break;
    default:
      break;
  }
  if(pX<0) {
    pW+=pX;
    pX = 0;
  }

  if(pY<0) {
    pH+=pY;
    pY = 0;    
  }

  if(IsYOutOfRange(pY) || pH<=0 || IsYOutOfRange(pX) || pW<=0 ) return;
  
  if(pX+pW>_buffer.GetWidth())
    pW = _buffer.GetWidth()-pX;
  if(pY+pH>_buffer.GetHeight())
    pH = _buffer.GetHeight()-pY;
printf("Draw Rect (Adjusted) %d,%d (%d, %d)\n", (int)pX, (int)pY, (int)pW, (int)pH);
  for(int16_t y = pY; y < pY+pH; y++)
    for(int16_t x = pX; x < pX+pW; x++) {
        DrawPixelUnchecked(x,y,pColor);
    }
}
