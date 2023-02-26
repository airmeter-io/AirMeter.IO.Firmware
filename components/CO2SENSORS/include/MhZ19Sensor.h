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


    char _rxBuf[32];

    char CalcChecksum(char *pPacket);

    void ReadSoftwareVersion();
    void ReadDetectionRange();
    void ReadTemperatureAdjustment();
    void ReadABCStatus();
    void ReadIDString();
    bool ExecuteCommand(MHZ19Buffer& pCommand, MHZ19Buffer& pResponse);
    Value _valAbcBase = { .i = 0 };
    Value _valAbcTick = { .i = 0 };
    Value _valAbcCount = { .i = 0 };
    Value _valTemp = { .i = 0 };
    Value _valTempAdjustment = { .i = 0 };

    const std::string SOURCE_NAME = "MHZ19x";
protected: 
    const std::string& GetValuesSourceName() const override;   
public:
    MhZ19Sensor(PinSerial* pSerial);

    bool RefreshValues() override;


    void DisableABC() override;
    void ManualCalibration(int pBaseLinePPM) override;
    void EnableABC(int pBaseLinePPM, int pNumberOfHoursPerCycle) override;

};