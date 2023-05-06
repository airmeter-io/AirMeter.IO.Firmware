#pragma once

#include "Common.h"
#include "EpdSpi.h"
#include "EPDBackBuffer.h"
#include "GpioManager.h"

enum SSD1680RamDataEntryMode : uint8_t {
    XDecreaseYDescrease = 0,
    XIncreaseYDecrease = 1,
    XDecreaseYIncrease = 2,
    XIncreaseYIncrease = 3
};

typedef struct {
    bool Enable;
    uint16_t DataLength;
    uint8_t *Data;
    int GateDrivingVoltage;
    struct {
        int Vsh1;
        int Vsh2;
        int Vsl;
    } SourceDrivingVoltage;
    uint8_t VCOM;

} SSD1680Lut;

enum SSD1680Cmd : uint8_t {
  SetGateDrivingVoltage = 0x03, // Really?
  SetSourceDrivingVoltage = 0x04, // Really?
  EnterDeepSleep = 0x10,
  ResetAllCommandsAndParameter = 0x12,
  WriteVCOMRegister = 0x2c,
  SendLUT = 0x32,
  WriteRegisterForDisplayOption = 0x37,
  SetBorderWaveForm = 0x3c,
  EndOptionLUT = 0x3F,
  ActivateDisplayUpdateSequence = 0x20,
  DisplayUpdateSequence = 0x22,
  WriteToBWRam = 0x24,
  WriteToRedRam = 0x26,
  DefineDataEntrySequence = 0x11,
  SetRamXAddressRange = 0x44,
  SetRamYAddressRange = 0x45,
  SetRamXAddressCounter = 0x4e,
  SetRamYAddressCounter = 0x4f,
};

enum SSD1680LutEndOption : uint8_t {
    Normal  = 0x22,
    SourceOutputLevelKeep = 0x07
};

enum SSD1306SleepMode : uint8_t {
    NormalMode = 0,
    DeepSleepMode1 = 0x1,
    DeepSleepMode2 = 0x11,
};

enum SSD1680DisplayUpdateSequence : uint8_t {
    ClockOn = 0x80,
    ClockOff = 0x01,
    ClockOnAnalogOn = 0xC0,
    AnalogOffClockOff = 0x03,
    ClockOnLoadLutMode1ClockOff = 0x91,
    ClockOnLoadLutMode2ClockOff = 0x99,
    ClockOnLoadTempLoadLutMode1ClockOff = 0xB1,    
    ClockOnLoadTempLoadLutMode2ClockOff = 0xB9,   
    ClockOnAnalogOnMode1AnalogOffOSCOff = 0XC7,
    ClockOnAnalogOnMode2AnalogOffOSCOff = 0XCF,
    ClockOnAnalogOnLoadTempMode1AnalogOffOSCOff = 0xF7,
    ClockOnAnalogOnLoadTempMode2AnalogOffOSCOff = 0xFF
};



class SSD1680 {
    EpdSpi& _spi;  
    std::vector<gpio_num_t> _gpios;
    GpioGroup _group;
    void SetRamArea(uint8_t pXStart, uint8_t pXEnd, uint16_t pYStart, uint16_t pYEnd, uint16_t pXOffset);
    void SetRamPointer(uint8_t pX, uint16_t pY, uint16_t pXOffset);
    void WaitBusy(const char* message, uint16_t busy_time);
    void WaitBusy2(const char* message, uint16_t busy_time);
    void WaitBusy(const char* message);

public:
    SSD1680(EpdSpi& pSpi);
    ~SSD1680();

    void ResetAll();
    void SetRamDataEntryMode(SSD1680RamDataEntryMode pMode, uint16_t pWidth, uint16_t pHeight, uint16_t pXOffset);
    void SetSleepMode(SSD1306SleepMode pMode);
    void WriteToBWRam(EPDBackBuffer& pBuffer);
    void WriteToRedRam(EPDBackBuffer& pBuffer);
    void ActivateDisplayUpdateSequence(int pWaitMs);
    void ActivateDisplayUpdateSequence2(int pWaitMs);
    void SetDisplayUpdateSequence(SSD1680DisplayUpdateSequence pSequence);
    void SendLUT(const SSD1680Lut& pLut);
    void SendEndLUT(SSD1680LutEndOption pOption);
    void SetGateDrivingVoltage(SSD1680Lut& pLut);
    void SetSourceDrivingVoltage(SSD1680Lut& pLut);
    void SetVCOMRegister(SSD1680Lut& pLut);
    void EnablePingPong();
    void SetBorderWaveForm(uint8_t pValue);
    
};