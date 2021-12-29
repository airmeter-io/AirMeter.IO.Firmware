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
    bool _isHeatingUp = false;
    bool _hasError = false;
    bool _isAbcEnabled = false;
    std::string _deviceName = "SCD30 CO2 Sensor";
    std::string _softVer = "Unknown";
    std::string _serialNo =  "Unknown";
    int _ppm = 0;
    int _maxPPM = 5000;
    static const std::vector<int> availablePPMs;
    CO2SensorValue _valTemperature = { .type = Double, .value = { .i = 0}, .name = "Temperature" };
    CO2SensorValue _valHumidity = { .type = Double, .value = { .i = 0}, .name = "Humidity" };  
    CO2SensorValue _valMeasurementInterval = { .type = Int, .value = { .i = 0}, .name = "Measurement Interval" };  
    CO2SensorValue _valTemperatureOffset = { .type = Int, .value = { .i = 0}, .name = "Temperature Offset" };  
    CO2SensorValue _valAltitude = { .type = Int, .value = { .i = 0}, .name = "Altitude" };  
    bool ReadCommand(Scd30I2CCommand pCommand, uint8_t* pParamData, uint pNumParamBytes, void * pResponseData, uint pNumResponseBytes);
    void ReadSerialNo();
    void ReadSoftwareVersion();    
    void ReadMeasurementInterval();      
    void ReadABCStatus();  
    void ReadTemperatureOffset();  
    void ReadAltitude();  
    
    uint Crc8(void *pData, size_t pLength);
public:
    Scd30Sensor(I2CDeviceSession* pSession);
    
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
};