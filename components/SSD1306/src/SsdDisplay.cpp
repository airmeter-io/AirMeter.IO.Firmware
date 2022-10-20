// #include "SsdDisplay.h"
// #define TAG "SsdDisplay"

// SsdDisplay::SsdDisplay(I2C& pI2C, uint8_t pWidth, uint8_t pHeight) : _i2c(pI2C), _width(pWidth), _height(pHeight), _frameBufferSize(pWidth*pHeight/8) {
//     _dev =  {
//         .i2c_port = pI2C.GetI2CPort(),
//         .i2c_addr = SSD1306_I2C_ADDR_0,
//         .screen = SSD1306_SCREEN, // or SH1106_SCREEN
//         .width = pWidth,
//         .height = pHeight};
//     int err = ssd1306_init(&_dev);
// 	if(err<0)  ESP_LOGI(TAG, "could not initialise display"); ;
//     _frameBuffer = (uint8_t*)malloc(_frameBufferSize);
//     Clear();

//     ssd1306_set_whole_display_lighting(&_dev, false);

// }

// SsdDisplay::~SsdDisplay() {
//     free(_frameBuffer);
// }

// void SsdDisplay::On() {
//     if(ssd1306_display_on(&_dev, true) < 0)  ESP_LOGI(TAG,"Could not switch on screen"); 
// }
// void SsdDisplay::Off() {
//     if(ssd1306_display_on(&_dev, false) < 0) ESP_LOGI(TAG,"Could not switch off screen"); 
// }

// int SsdDisplay::GetWidth() {
//     return _width;
// }

// int SsdDisplay::GetHeight() {
//     return _height;
// }

// void SsdDisplay::Clear() {
//     memset(_frameBuffer, 0, _frameBufferSize);
// }

// void SsdDisplay::DrawText(FontSize pFontSize, uint8_t pX, uint8_t pY, const char *pStr, ...)
// {
//     const font_info_t * font = nullptr;
    
//     switch(pFontSize) {
//         case Small :
//             font =  font_builtin_fonts[0];
//             break;
//         case Medium : 
//             font =  font_builtin_fonts[FONT_FACE_TERMINUS_BOLD_10X18_ISO8859_1];
//             break;
//         case Large :
//             font =  font_builtin_fonts[FONT_FACE_TERMINUS_BOLD_16X32_ISO8859_1];
//             break;
//     }
// 	char buffer[50];
// 	va_list arg;
// 	va_start(arg, pStr);
// 	vsnprintf(buffer, sizeof(buffer), pStr, arg);
// 	va_end(arg);

//     if(ssd1306_draw_string(&_dev, _frameBuffer,font,pX, pY, buffer, OLED_COLOR_WHITE, OLED_COLOR_BLACK)<0) ESP_LOGI(TAG,"Could not draw text");  
// } 



// void SsdDisplay::FillRect(int8_t pX, int8_t pY, uint8_t pW, uint8_t pH, ssd1306_color_t pColor) {
//     ssd1306_fill_rectangle(&_dev, _frameBuffer, pX, pY, pW, pH, pColor);
// }

// void SsdDisplay::LoadXBM(const uint8_t *pImageBits) {
//     if (ssd1306_load_xbm(&_dev, (uint8_t *)pImageBits, _frameBuffer))
//         ESP_LOGI(TAG,"could not initialise display");  
  

// }


// void SsdDisplay::Present() {
//     if(ssd1306_load_frame_buffer(&_dev, _frameBuffer)< 0) ESP_LOGI(TAG,"Could not present");  
// }