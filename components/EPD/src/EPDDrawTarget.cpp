
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

// Taken from Adafruit GFX
void EPDDrawTarget::DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                             EPDColor color) {
#if defined(ESP8266)
  yield();
#endif
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    Swap(x0, y0);
    Swap(x1, y1);
  }

  if (x0 > x1) {
    Swap(x0, x1);
    Swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// Taken/adaptedfrom Adafruit GFX
void EPDDrawTarget::DrawDottedLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                             EPDColor color) {
  int drawThis = 0;                              
#if defined(ESP8266)
  yield();
#endif
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    Swap(x0, y0);
    Swap(x1, y1);
  }

  if (x0 > x1) {
    Swap(x0, x1);
    Swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      if(drawThis%3) 
        drawPixel(y0, x0, color);
      drawThis++;
    } else {
      if(drawThis%3)
        drawPixel(x0, y0, color);
      drawThis++;
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void EPDDrawTarget::DrawFilledRectangle(int16_t pX, int16_t pY, int16_t pW, int16_t pH, EPDColor pColor) {
 
  AdjustCoOrdinatesForRotation(pX,pY);
  switch (_rotation)
  {
    case 1:
      pX -= pH;
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
  for(int16_t y = pY; y < pY+pH; y++)
    for(int16_t x = pX; x < pX+pW; x++) {
        DrawPixelUnchecked(x,y,pColor);
    }
}
