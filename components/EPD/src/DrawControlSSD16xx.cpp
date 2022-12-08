#include "DrawControlSSD16xx.h"
#include "EPDDrawTarget.h"


DrawControlSSD16xx::DrawControlSSD16xx(SSD1680& pSsd1680,const SSD16xxConfiguration& pConfig) : _ssd1680(pSsd1680), _config(pConfig) {
    _backBuffer = new EPDBackBuffer(pConfig.Width, pConfig.Height);
    _backBuffer->Fill(White);
    _gfx = new EPDDrawTarget(*_backBuffer);

    bool foundErr = false;
    if(pConfig.LUTPartialUpdate.DataLength!=LUTPartialUpdate.DataLength) {
        foundErr = true;
        printf("Lut data length doesn't match %d, %d\n",(int)pConfig.LUTPartialUpdate.DataLength, (int)LUTPartialUpdate.DataLength );
    } else {
        if(pConfig.LUTPartialUpdate.GateDrivingVoltage!=LUTPartialUpdate.GateDrivingVoltage) {
            foundErr = true;
            printf("Lut  GateDrivingVoltage doesn't match %d, %d\n",(int)pConfig.LUTPartialUpdate.GateDrivingVoltage, (int)LUTPartialUpdate.GateDrivingVoltage );
        }
        if(pConfig.LUTPartialUpdate.VCOM!=LUTPartialUpdate.VCOM) {
            foundErr = true;
            printf("Lut VCOM doesn't match %d, %d\n",(int)pConfig.LUTPartialUpdate.VCOM, (int)LUTPartialUpdate.VCOM );
        }

        if(pConfig.LUTPartialUpdate.SourceDrivingVoltage.Vsh1!=LUTPartialUpdate.SourceDrivingVoltage.Vsh1) {
            foundErr = true;
            printf("Lut Vsh1 doesn't match %d, %d\n",(int)pConfig.LUTPartialUpdate.SourceDrivingVoltage.Vsh1, (int)LUTPartialUpdate.SourceDrivingVoltage.Vsh1 );
        }

        if(pConfig.LUTPartialUpdate.SourceDrivingVoltage.Vsh2!=LUTPartialUpdate.SourceDrivingVoltage.Vsh2) {
            foundErr = true;
            printf("Lut Vsh2 doesn't match %d, %d\n",(int)pConfig.LUTPartialUpdate.SourceDrivingVoltage.Vsh2, (int)LUTPartialUpdate.SourceDrivingVoltage.Vsh2 );
        }

        if(pConfig.LUTPartialUpdate.SourceDrivingVoltage.Vsl!=LUTPartialUpdate.SourceDrivingVoltage.Vsl) {
            foundErr = true;
            printf("Lut Vsl doesn't match %d, %d\n",(int)pConfig.LUTPartialUpdate.SourceDrivingVoltage.Vsl, (int)LUTPartialUpdate.SourceDrivingVoltage.Vsl );
        }

        for(auto i = 0; i < pConfig.LUTPartialUpdate.DataLength; i++) {
            if(pConfig.LUTPartialUpdate.Data[i]!=LUTPartialUpdate.Data[i]) {
                foundErr = true;
                printf("LUT byte %d does not match %X!=%X\n", i, (int) pConfig.LUTPartialUpdate.Data[i], (int)LUTPartialUpdate.Data[i]);
            }
        }
    }

    if(foundErr) {
        printf("Errors in LUT (%d, %d)\n", pConfig.Width, pConfig.Height);
    }
    else {
         printf("No Errors in LUT (%d, %d)\n", pConfig.Width, pConfig.Height);
    }
}

DrawControlSSD16xx::~DrawControlSSD16xx() {
    delete _gfx;
}

uint8_t rawLutData[153] = {
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
    };

SSD1680Lut DrawControlSSD16xx::LUTPartialUpdate = {   
    .Enable = true,
    .DataLength = 153, 
    .Data = rawLutData,
    .GateDrivingVoltage = 200, // 20.0V
    .SourceDrivingVoltage = {
        .Vsh1 = 150, // 15.0V
        .Vsh2 = 58,   // Forces 0 value which from SSD1680's datasheet is an unsupported value
        .Vsl = -150  // -15.0V
    },
    .VCOM = 0x32 // This does not seem to be a supported value either!
};

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
        _ssd1680.WriteToBWRam(*_backBuffer);
        _ssd1680.WriteToRedRam(*_backBuffer);
        _ssd1680.SetDisplayUpdateSequence(SSD1680DisplayUpdateSequence::ClockOnAnalogOnLoadTempMode2AnalogOffOSCOff);
        _ssd1680.ActivateDisplayUpdateSequence2(300);
        vTaskDelay(300 / portTICK_PERIOD_MS);
        _ssd1680.SetSleepMode(SSD1306SleepMode::DeepSleepMode2);
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