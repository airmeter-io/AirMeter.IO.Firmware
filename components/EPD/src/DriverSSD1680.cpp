#include "DriverSSD1680.h"
#include "EPDDrawTarget.h"


DriverSSD1680::DriverSSD1680(SSD1680& pSsd1680) : _ssd1680(pSsd1680) {
    _backBuffer->Fill(White);
    _gfx = new EPDDrawTarget(*_backBuffer);
}

DriverSSD1680::~DriverSSD1680() {
    delete _gfx;
}

SSD1680Lut DriverSSD1680::LUTPartialUpdate = {    
    .Data = {
0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 
0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
0x80, 0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 
0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 
0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 
0x00, 0x00, 0x00,  

    },
    .GateDrivingVoltage = 200, // 20.0V
    .SourceDrivingVoltage = {
        .Vsh1 = 150, // 15.0V
        .Vsh2 = 58,   // Forces 0 value which from SSD1680's datasheet is an unsupported value
        .Vsl = -150  // -15.0V
    },
    .VCOM = 0x32 // This does not seem to be a supported value either!
};

void DriverSSD1680::RenderToDisplay(bool pFastUpdate) {
    if(pFastUpdate) {
        _ssd1680.ResetAll();
        _ssd1680.SetRamDataEntryMode(XIncreaseYIncrease, WIDTH-1, HEIGHT);

        _ssd1680.SendLUT(LUTPartialUpdate);
        _ssd1680.SendEndLUT(SSD1680LutEndOption::Normal);
    
      //  _ssd1680.EnablePingPong();
        _ssd1680.SetBorderWaveForm(0x80);
        _ssd1680.WriteToBWRam(*_backBuffer);
        _ssd1680.WriteToRedRam(*_backBuffer);
        _ssd1680.SetDisplayUpdateSequence(SSD1680DisplayUpdateSequence::ClockOnAnalogOnLoadTempMode2AnalogOffOSCOff);
        _ssd1680.ActivateDisplayUpdateSequence2(300);
        vTaskDelay(300 / portTICK_PERIOD_MS);
        _ssd1680.SetSleepMode(SSD1306SleepMode::DeepSleepMode2);
    } else {
        _ssd1680.ResetAll();
        _ssd1680.SetRamDataEntryMode(XIncreaseYIncrease, WIDTH-1, HEIGHT);
        _ssd1680.WriteToBWRam(*_backBuffer);
         _ssd1680.WriteToRedRam(*_backBuffer);
          _ssd1680.SetDisplayUpdateSequence(SSD1680DisplayUpdateSequence::ClockOnAnalogOnLoadTempMode1AnalogOffOSCOff);
        _ssd1680.ActivateDisplayUpdateSequence(1200);
        _ssd1680.SetSleepMode(SSD1306SleepMode::DeepSleepMode1);
    }
}
DrawTarget* DriverSSD1680::GetDrawTarget() {
    return _gfx;
}