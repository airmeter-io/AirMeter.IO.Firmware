// #pragma once
// #include "Common.h"
// #include "I2C.h"

// enum FontSize { Small, Medium, Large };

// class SsdDisplay {
//     I2C& _i2c;    
//     int _width;
//     int _height;
//     int _frameBufferSize;
//     ssd1306_t _dev;
//     uint8_t* _frameBuffer;
// public:
//     SsdDisplay(I2C& pI2C, uint8_t pWidth, uint8_t pHeight);
//     ~SsdDisplay();

//     int GetWidth();
//     int GetHeight();

//     void On();
//     void Off();
//     void Clear();

//     void DrawText(FontSize pFontSize, uint8_t pX, uint8_t pY, const char *pStr, ...);
//     void FillRect(int8_t pX, int8_t pY, uint8_t pW, uint8_t pH, ssd1306_color_t pColor);

//     void LoadXBM(const uint8_t *pImageBits);
//     void Present();

// };