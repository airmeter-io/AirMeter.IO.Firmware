#pragma once
#include "Common.h"
#include "PinSerial.h"
#include "CO2Sensor.h"

#define MAX_MESSAGE_SIZE 20

class CubicCO2Buffer;

enum CubicCO2Command {
    ReadValues = 0x1,
    StartCalibration = 0x03,
    ReadRealData = 0x4,
    SetUnknownMode = 0x6,
    GetSensorInfo = 0x0D,
    SetSensorInfo = 0x0E,
    GetABCInfo = 0xF,
    SetABCInfo = 0x10,
    GetSoftwareVerison = 0x1e,
    GetSerialNo = 0x1f,
    SetSerialNo = 0x23,
    GetInternalStatusText = 0xFA,
    StartStopStreamMode = 0xFD
};

enum CubicCO2PacketType {
   Command = 0x11,
   Response = 0x16,
   Error = 0x06
};

enum CubicCO2ErrorCode {
   Unknown = -3,
   RetryTimeout = -2,
   ReceivedCommand = -1,
   Success = 0,
   Checksum = 2
};


class CubicCO2Sensor : public CO2Sensor {
    PinSerial* _serial;
    char _rxBuf[32];
    std::string _rawValStatus;
    std::string _unknownStatus;
    std::string _modeStatus;
    Value _valDrift = {  .b = false };
    Value _valLightAging = { .b = false };
    Value _valCalibrated = { .b = false };
    Value _valHasLessThanRange = { .b = false };
    Value _valHasGreaterThanRange = { .b = false };
    Value _valAbcBase = { .b = false };
    Value _valRawValStatus = { .s = &_rawValStatus }; 
    Value _valUnknownStatus= { .s = &_unknownStatus }; 
    Value _valModeStatus= { .s = &_modeStatus }; 


    CubicCO2ErrorCode ExecuteCommand(CubicCO2Buffer& pInput, CubicCO2Buffer& pOutput, int pTries=3);
    const std::string SOURCE_NAME = "CubicCM11xx";

    void ReadSoftwareVersion();
    void ReadSerialNo();
    void ReadAbcInfo();
    void ReadSensorInfo();
    void SetAbcParameters(bool pOpen, int pCycle, int pBaseCO2Value);
    void StartCalibration(int pBaseCO2Value);
protected: 
    const std::string& GetValuesSourceName() const override;
public:
    CubicCO2Sensor(PinSerial* pSerial);

    bool RefreshValues() override;

    
  
    void DisableABC() override;
    void ManualCalibration(int pBaseLinePPM) override;
    void EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) override;


    void ReadSensorStatusText();
    void SetUnknownMode(uint8_t pMode);
    void SetSerialNumber(short pPart1, short pPart2, short pPart3, short pPart4, short pPart5);

};