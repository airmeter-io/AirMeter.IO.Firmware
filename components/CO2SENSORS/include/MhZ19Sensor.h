#pragma once
#include "Common.h"
#include "PinSerial.h"
#include "CO2Sensor.h"
#include<string>

class MHZ19Buffer;

enum MHZ19Command {
    RecoveryReset = 0x78,
    SetAutoCalibration = 0x79,
    GetAutoCalibration = 0x7D,
    Unknown7E = 0x7E,
    ReadRawCO2 = 0x84,
    ReadUnlimittedCO2 = 0x85,
    ReadCO2 = 0X86,
    ZeroCalibration = 0x87,
    SpanCalibration = 0x88,
    Unknown90 = 0x90,
    Unknown91 = 0x91,
    Unknown92 = 0x92,
    Unknown93 = 0x93,
    Unknown94 = 0x94,
    ReadIDString = 0x95,
    SetDetectionRange = 0x99,
    Unknown9A = 0x9A,
    GetDetectionRange = 0x9B,
    GetBackgroundCO2 = 0x9C,
    Unknown9F = 0x9F,
    GetFirmwareVersion = 0xA0,
    UnknownA1 = 0xA1,
    GetLastResponse = 0xA2,
    GetTemplCalibration = 0xA3,
    UnknownAC = 0xAC,
    UnknownAD = 0xAD,
    UnknownAE = 0xAE,
    UnknownAF = 0xAF
};

class MhZ19Sensor : public CO2Sensor {
    PinSerial* _serial;
    int _ppm;
    int _maxPPM;
    bool _isHeatingUp = false;
    bool _hasError = false;
    char _rxBuf[32];
    bool _isAbcEnabled = false;

    char CalcChecksum(char *pPacket);
    std::string _serialNo  = "-";
    std::string _softVer  = "-";
    std::string _deviceName = "Winsen MH-Zxx Family CO2 Sensor";
    void ReadSoftwareVersion();
    void ReadDetectionRange();
    void ReadTemperatureAdjustment();
    void ReadABCStatus();
    void ReadIDString();
    bool ExecuteCommand(MHZ19Buffer& pCommand, MHZ19Buffer& pResponse);
    CO2SensorValue _valAbcBase = { .type = Int, .value = { .i = 0}, .name = "ABC Base" };
    CO2SensorValue _valAbcTick = { .type = Int, .value = { .i = 0}, .name = "ABC Tick" };
    CO2SensorValue _valAbcCount = { .type = Int, .value = { .i = 0}, .name = "ABC Count" };
    CO2SensorValue _valTemp = { .type = Int, .value = { .i = 0}, .name = "Temperature" };
    CO2SensorValue _valTempAdjustment = { .type = Int, .value = { .i = 0}, .name = "Temp Adjustment" };
    
public:
    MhZ19Sensor(PinSerial* pSerial);
    std::string& GetSerialNo()  override;
    std::string& GetDeviceName() override;
    std::string& GetSWVersion() override;
    bool RefreshValues() override;
    int GetPPM() override;
    const std::vector<int>& GetAvailableMaxPPM() const override;
    void SetMaxPPM(int pMaxPPM) override; 
    int GetMaxPPM() override;
    bool GetIsHeatingUp() override;
    bool GetHasError() override;
    int GetBasePPM() override;
    int GetDaysPerABCCycle() override;
    bool GetIsABCEnabled() override;
    void DisableABC() override;
    void ManualCalibration(int pBaseLinePPM) override;
    void EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) override;
    int GetMinBasePPM() override;
    int GetMaxBasePPM();
    int GetMinDaysPerABCCycle();
    int GetMaxDaysPerABCCycle();
};