#include "Common.h"
#include "SenseairBase.h"
#include "I2CDeviceSession.h"



class SenseairI2CSensor : public SenseairBase {
    I2CDeviceSession*  _session;
    bool _hasDataRestore = false;
    ISensorManager *_sensorManager;
    uint8_t _sensorStateData[25] = { 0x1 };
    const std::string SOURCE_NAME = "SenseairI2C";;
    void ReadFirmwareVersion();
    void ReadSensorID();
    void ReadCalibrationStatus();
    void ReadMeasurementModeInfo();

    bool WriteCalibrationStatus(uint8_t pStatus);
    bool WriteCalibrationCommand(uint8_t pByte1, uint8_t pByte2);

    bool WriteRegister(uint8_t pRegAddr, const uint8_t *pRegData, uint32_t pLen);
    bool ReadRegister(uint8_t pRegAddr, uint8_t *pRegData, uint32_t pLen);
protected: 
    const std::string& GetValuesSourceName() const override;   
public:
    SenseairI2CSensor(I2CDeviceSession* pSession, ISensorManager *pSensorManager);
    ~SenseairI2CSensor();

    
    bool RefreshValues() override;
   
    void DisableABC() override;
    void ManualCalibration(int pBaseLinePPM) override;
    void EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) override;
};