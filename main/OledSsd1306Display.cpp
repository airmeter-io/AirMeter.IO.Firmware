#include "OledSsd1306Display.h"


#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

Oledssd1306Display::Oledssd1306Display(GeneralSettings& pSettings, WifiTask& pWifi, I2C& pI2C) : CO2MonitorDisplay(pSettings), _wifi(pWifi), _i2c(pI2C) {
    auto count = pI2C.GetNumDevices();
    memset(_lastPPM,0, sizeof(_lastPPM));
    memset(_lastTemp, 0, sizeof(_lastTemp));
    memset(_lastPressure, 0, sizeof(_lastPressure));
    memset(_lastHumdity, 0, sizeof(_lastHumdity));
    for(auto i = 0; i < count; i++) {
        if(pI2C.GetDevice(i) == 0x3c)
        {
            // _ssd = new SsdDisplay(_i2c,DISPLAY_WIDTH, DISPLAY_HEIGHT);
            // _ssd->Clear();
            // _ssd->LoadXBM(LogoImage);
            // _ssd->Present();
            // _ssd->On();
            // break;
        }        
    }
}

void Oledssd1306Display::RenderReadings() {
    // if(!_ssd) return;
    // char ppm[10];
    // if(_values.CO2)
    //     snprintf(ppm, sizeof(ppm), "%d", _values.CO2->GetPPM());
    // else   
    //     ppm[0] = 0;
    
    // char *temp = _values.Bme280.GetTemperatureStr();
    // char *pressure = _values.Bme280.GetPressureStr(1);
    // char *humidity = _values.Bme280.GetHumidityStr();
    // if( _lastRenderType != Readings || 
    //     strcpy(ppm, _lastPPM) || 
    //     strcpy(temp, _lastTemp) || 
    //     strcpy(pressure, _lastPressure) || 
    //     strcpy(humidity, _lastHumdity) ) {
    //     strlcpy(_lastPPM, ppm, sizeof(_lastPPM));
    //     strlcpy(_lastTemp, temp, sizeof(_lastTemp));
    //     strlcpy(_lastPressure, pressure, sizeof(_lastPressure));
    //     strlcpy(_lastHumdity, humidity, sizeof(_lastHumdity));

    //     _ssd->Clear();

    //     if(strlen(ppm)>0) {
    //         _ssd->DrawText(FontSize::Large,0,0,  "%s",ppm);
    //         _ssd->DrawText(FontSize::Medium,0,28, "ppm");
    //     }
    //      _ssd->DrawText(FontSize::Medium,128-((strlen(temp)+1)*10),0, "%sC",temp);
    //      _ssd->DrawText(FontSize::Medium,128-((strlen(humidity)+1)*10),17, "%s%c",humidity,'%');
    //      _ssd->DrawText(FontSize::Medium,128-((strlen(pressure)+3)*10),34, "%shPa",pressure);
    //      _ssd->DrawText(FontSize::Small,2,57, "IP %s",_wifi.GetManager().GetIPAddress().c_str());

    //     _ssd->Present();
    // }
}

void Oledssd1306Display::RenderAccessPointInfo(const std::string& pSSID, const std::string& pPassword) {
    // if(!_ssd) return;
    //   //qrCode.Display(*_display,"WIFI");//:T:WPA;S:CO2Device5BC4720255BE;P:12345678901234567901234567890;;" ) ;
    // _ssd->Clear();
    // _ssd->DrawText(FontSize::Small,2,2, "%s",pSSID.c_str());
    // _ssd->DrawText(FontSize::Medium,2,14, "%.12s",pPassword.c_str());
    // _ssd->DrawText(FontSize::Medium,2,34, "%.12s",pPassword.c_str()+12);        
    // _ssd->Present();
}

bool Oledssd1306Display::IsPresent() {
    return false;//_ssd!=nullptr;
}

bool Oledssd1306Display::SupportsHalfOn() {
    return false;
}

void Oledssd1306Display::On(bool pHalf) {
   // _ssd->On();
}

void Oledssd1306Display::Off() {
  //  _ssd->Off();
}