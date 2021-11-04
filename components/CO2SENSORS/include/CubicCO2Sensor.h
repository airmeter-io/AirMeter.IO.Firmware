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
    std::string _softVer;
    std::string _serialNo;
    std::string _rawValStatus;
    std::string _unknownStatus;
    std::string _modeStatus;
   CO2SensorValue _valDrift = { .type = Bool, .value = { .b = false}, .name = "Drift" };
    CO2SensorValue _valLightAging = { .type = Bool, .value = { .b = false}, .name = "LightAging" };
    CO2SensorValue _valCalibrated = { .type = Bool, .value = { .b = false}, .name = "Calibrated" };
    CO2SensorValue _valHasLessThanRange = { .type = Bool, .value = { .b = false}, .name = "Has less than range" };
    CO2SensorValue _valHasGreaterThanRange = { .type = Bool, .value = { .b = false}, .name = "Has greater than range" };
    CO2SensorValue _valAbcOpen = { .type = Bool, .value = { .b = false}, .name = "ABC Open" };
    CO2SensorValue _valAbcCycle = { .type = Int, .value = { .i = 0}, .name = "ABC Cycle" };
    CO2SensorValue _valAbcBase = { .type = Int, .value = { .b = false}, .name = "ABC Base" };
    CO2SensorValue _valRawValStatus = { .type = String, .value = { .s = &_rawValStatus}, .name = "Raw Value Status" }; 
    CO2SensorValue _valUnknownStatus= { .type = String, .value = { .s = &_unknownStatus}, .name = "Unknown Status" }; 
    CO2SensorValue _valModeStatus= { .type = String, .value = { .s = &_modeStatus}, .name = "Mode Status" }; 
    bool _isHeatingUp = false;
    bool _hasError = false;
    std::string _deviceName = "Cubic CMxxxx Family CO2 Sensor";
    int _ppm = 0;
    int _maxPPM;
    static const std::vector<int> availablePPMs;
    CubicCO2ErrorCode ExecuteCommand(CubicCO2Buffer& pInput, CubicCO2Buffer& pOutput, int pTries=3);
    

    void ReadSoftwareVersion();
    void ReadSerialNo();
    void ReadAbcInfo();
    void ReadSensorInfo();
    void SetAbcParameters(bool pOpen, int pCycle, int pBaseCO2Value);
    void StartCalibration(int pBaseCO2Value);
public:
    CubicCO2Sensor(PinSerial* pSerial);
    
    std::string& GetSerialNo()  override;
    std::string& GetDeviceName() override;
    std::string& GetSWVersion() override;

    bool RefreshValues() override;
    int GetPPM() override;
    bool GetIsHeatingUp() override;
    bool GetHasError() override;
    const std::vector<int>& GetAvailableMaxPPM() const override;
    void SetMaxPPM(int pMaxPPM) override;   
    int GetMaxPPM() override;
    
    int GetBasePPM() override;
    int GetDaysPerABCCycle() override;
    bool GetIsABCEnabled() override;
    void DisableABC() override;
    void ManualCalibration(int pBaseLinePPM) override;
    void EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) override;


    void ReadSensorStatusText();
    void SetUnknownMode(uint8_t pMode);
    void SetSerialNumber(short pPart1, short pPart2, short pPart3, short pPart4, short pPart5);

};