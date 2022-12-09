#include "DrawControlSSD16xx.h"
#include "EPDDrawTarget.h"


DrawControlSSD16xx::DrawControlSSD16xx(SSD1680& pSsd1680,const SSD16xxConfiguration& pConfig) : _ssd1680(pSsd1680), _config(pConfig) {
    _backBuffer = new EPDBackBuffer(pConfig.Width, pConfig.Height);
    _backBuffer->Fill(White);
    _gfx = new EPDDrawTarget(*_backBuffer);    
}

DrawControlSSD16xx::~DrawControlSSD16xx() {
    delete _gfx;
}



void DrawControlSSD16xx::RenderToDisplay(bool pFastUpdate) {
    if(pFastUpdate) {
        _ssd1680.ResetAll();
        _ssd1680.SetRamDataEntryMode(XIncreaseYIncrease, _config.Width-_config.RamXOffset, _config.Height, _config.RamXOffset);
        if(_config.LUTPartialUpdate.Enable) {
            _ssd1680.SendLUT(_config.LUTPartialUpdate);
            _ssd1680.SendEndLUT(SSD1680LutEndOption::Normal);
        }
        if(_config.EnablePingPong)
            _ssd1680.EnablePingPong();
        _ssd1680.SetBorderWaveForm(0x80);
        esp_task_wdt_reset();
        _ssd1680.WriteToBWRam(*_backBuffer);
        _ssd1680.WriteToRedRam(*_backBuffer);
        _ssd1680.SetDisplayUpdateSequence(SSD1680DisplayUpdateSequence::ClockOnAnalogOnLoadTempMode2AnalogOffOSCOff);
        _ssd1680.ActivateDisplayUpdateSequence2(300);
        esp_task_wdt_reset();
        vTaskDelay(300 / portTICK_PERIOD_MS);
        esp_task_wdt_reset();
        _ssd1680.SetSleepMode(SSD1306SleepMode::DeepSleepMode2);
        esp_task_wdt_reset();
    } else {
        _ssd1680.ResetAll();
        _ssd1680.SetRamDataEntryMode(XIncreaseYIncrease, _config.Width-_config.RamXOffset, _config.Height, _config.RamXOffset);
        _ssd1680.WriteToBWRam(*_backBuffer);
        _ssd1680.WriteToRedRam(*_backBuffer);
        _ssd1680.SetDisplayUpdateSequence(SSD1680DisplayUpdateSequence::ClockOnAnalogOnLoadTempMode1AnalogOffOSCOff);
        _ssd1680.ActivateDisplayUpdateSequence(1200);
        _ssd1680.SetSleepMode(SSD1306SleepMode::DeepSleepMode1);
    }
}
DrawTarget* DrawControlSSD16xx::GetDrawTarget() {
    return _gfx;
}