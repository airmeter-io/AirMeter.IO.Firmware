
#include "EPDDrawTarget.h"
EPDDrawTarget::EPDDrawTarget(EPDBackBuffer& pBuffer) : _buffer(pBuffer) {
  _rotation  = 0;
}

EPDDrawTarget::~EPDDrawTarget() {

}

void EPDDrawTarget::setRotation(int16_t pRotation) {
   _rotation = pRotation;
}

void inline swap(int16_t a, int16_t b) {
  auto c = a;
  a = b;
  b = a;
}

void EPDDrawTarget::drawPixel(int16_t x, int16_t y, uint16_t color) {
 

  // check rotation, move pixel around if necessary
  switch (_rotation)
  {
    case 1:
      swap(x, y);
      x = _buffer.GetWidth() - x - 1;
      break;
    case 2:
      x = _buffer.GetWidth() - x - 1;
      y = _buffer.GetHeight() - y - 1;
      break;
    case 3:
      swap(x, y);
      y = _buffer.GetHeight() - y - 1;
      break;
  }
   if ((x < 0) || (x >= _buffer.GetWidth()) || (y < 0) || (y >= _buffer.GetHeight())) return;
  uint16_t i = x / 8 + y *  _buffer.GetWidth() / 8;

  // This is the trick to draw colors right. Genious Jean-Marc
  if (color) {
    _buffer.GetBuffer()[i] = (_buffer.GetBuffer()[i] & (0xFF ^ (1 << (7 - x % 8))));
    } else {
    _buffer.GetBuffer()[i] = (_buffer.GetBuffer()[i] | (1 << (7 - x % 8)));
    }
}

void EPDDrawTarget::BltMonoBitmap(uint8_t* pData, Dimensions pBitmapSize, Position pDrawAt, Rectangle pClipRectangle, uint16_t pColor){
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
