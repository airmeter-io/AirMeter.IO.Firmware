#include "Common.h"
#include "SenseairBase.h"
#include "I2CDeviceSession.h"



class SenseairI2CSensor : public SenseairBase {
    I2CDeviceSession*  _session;
   
    void ReadFirmwareVersion();
    void ReadSensorID();
    void ReadCalibrationStatus();
    void ReadMeasurementModeInfo();

    bool WriteRegister(uint8_t pRegAddr, const uint8_t *pRegData, uint32_t pLen);
    bool ReadRegister(uint8_t pRegAddr, uint8_t *pRegData, uint32_t pLen);
    
public:
    SenseairI2CSensor(I2CDeviceSession* pSession);
    ~SenseairI2CSensor();


    bool RefreshValues() override;
   
    void DisableABC() override;
    void ManualCalibration(int pBaseLinePPM) override;
    void EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) override;
};