#include "Common.h"
#include "I2CDeviceSession.h"
#include "CO2Sensor.h"


enum Scd30I2CCommand : uint16_t 
{
      START_CONTINUOUS                        = 0x0010,
      STOP_CONTINUOUS                         = 0x0104, 
      SET_MEASUREMENT_INTERVAL                = 0x4600,
      GET_READY_STATUS                        = 0x0202,
      READ_MEASUREMENT                        = 0x0300,
      GET_OR_SET_ABC                          = 0x5306, 
      SET_EXTERNAL_REF_VALUE                  = 0x5204, 
      SET_TEMP_OFFSET                         = 0x5403, 
      ALTITUDE_COMPENSATION                   = 0x5102,
      READ_SERIAL_NUMBER                      = 0xD033,
      READ_FIRMWARE_REV                       = 0xD100,
      SOFT_RESET                              = 0xD304, 
};

class Scd30Sensor : public CO2Sensor {
    I2CDeviceSession* _session;


    Value _valTemperature = { .i = 0 };
    Value _valHumidity = { .i = 0 };  
    Value _valMeasurementInterval = { .i = 0 };  
    Value _valTemperatureOffset = { .i = 0 };  
    Value _valAltitude = { .i = 0 };  
    const std::string SOURCE_NAME = "SCD30";
    bool ReadCommand(Scd30I2CCommand pCommand, uint8_t* pParamData, uint pNumParamBytes, void * pResponseData, uint pNumResponseBytes);
    void ReadSerialNo();
    void ReadSoftwareVersion();    
    void ReadMeasurementInterval();      
    void ReadABCStatus();  
    void ReadTemperatureOffset();  
    void ReadAltitude();  
    void SetMeasurementInterval(uint16_t pIntervalSeconds);

    uint Crc8(void *pData, size_t pLength);
protected: 
    const std::string& GetValuesSourceName() const override;
public:
    Scd30Sensor(I2CDeviceSession* pSession);
    


    bool RefreshValues() override;

    

    void DisableABC() override;
    void ManualCalibration(int pBaseLinePPM) override;
    void EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) override;
};