#include "SenseairI2CSensor.h"
#include "I2CDeviceSessionCommand.h"

//
// Borrowed from https://github.com/Niclas73/Sunrise-master/blob/master/src/sunrise_i2c.h 
//
enum SENSEAIR_I2C_RERISTERS
{
      ERROR_STATUS                        = 0x00, // 2 bytes  Done!
      CO2_FILTERED_COMPENSATED            = 0x06, // 2 bytes  Done!
      CHIP_TETMP                          = 0x08, // 2 bytes  Done!
      MEASUREMENT_COUNT                   = 0x0D, // 1 byte   Done!
      MEASUREMENT_CYCLE_TIME              = 0x0E, // 2 byte   Done!
      CO2_UNFILTERED_COMPENSATED          = 0x10, // 2 bytes  Done!
      CO2_FILTERED_UNCOMPENSATED          = 0x12, // 2 bytes  Done!
      CO2_UNFILTERED_UNCOMPENSATED        = 0x14, // 2 bytes  Done!
      FIRMWARE_TYPE                       = 0x2F, // 1 byte
      FIRMWARE_REV                        = 0x38, // 2 byte
      SENSOR_ID                           = 0x3A, // 4 byte 
      CALIBRATION_STATUS                  = 0x81, // 1 byte   Done!
      CALIBRATION_COMMAND                 = 0x82, // 2 bytes  Done!
      CALIBRATION_TARGET                  = 0x84, // 2 bytes  Done!
      ABC_TIME                            = 0x88, // 2 bytes  Done! 
      ABC_0                               = 0x8A, // 2 byte
      ABC_1                               = 0x8C, // 2 byte
      ABC_2                               = 0x8E, // 2 byte
      ABC_3                               = 0x90, // 2 byte
      START_SINGLE_MEASUREMENT            = 0xC3, // 1 byte   Done!
      MEASUREMENT_MODE                    = 0x95, // 1 byte   Done!
      MEASUREMENT_PERIOD                  = 0x96, // 2 bytes  Done!
      NBR_SAMPLES                         = 0x98, // 2 bytes  Done!
      ABC_PERIOD                          = 0x9A, // 2 bytes  Done!
      CLEAR_ERROR_STATUS                  = 0x9D, // 1 byte   Done!
      ABC_TARGET                          = 0x9E, // 2 bytes  Done! 
      IIR_FILTER_PARMETER                 = 0xA1, // 1 byte   Done!
      RESET_SENSOR                        = 0xA3, // 1 byte   Done!
      METER_CONTROL                       = 0xA5, // 1 byte   Done! 
      I2C_ADDRESS                         = 0xA7, // 1 byte   Done!
      POWER_DOWN_DATA                     = 0xC4, // 24 byte
};


SenseairI2CSensor::SenseairI2CSensor(I2CDeviceSession* pSession) : _session(pSession) {
    _deviceName = "Senseair I2C";
    ReadFirmwareVersion();
    ReadSensorID();
    ReadCalibrationStatus();

    
    ReadMeasurementModeInfo();
}

SenseairI2CSensor::~SenseairI2CSensor() {
    delete _session;
}





bool SenseairI2CSensor::RefreshValues() {
    uint8_t ppmBytes[0xf];
    uint8_t errorStatus[2];
    if(ReadRegister((uint8_t )SENSEAIR_I2C_RERISTERS::CO2_FILTERED_COMPENSATED,ppmBytes, sizeof(ppmBytes))) {
        _ppm = ppmBytes[0]*256 + ppmBytes[1];
        ushort tempRaw = ppmBytes[2]*256 + ppmBytes[3];
        ushort measurementCount = ppmBytes[7];
        ushort measurementSeconds = ppmBytes[8]*256 + ppmBytes[9];
        measurementSeconds*=2;
       _temp = std::to_string(tempRaw/100)+"."+std::to_string(tempRaw % 100);

       _measurementInfo = std::to_string(measurementSeconds) + "s since reading #"+std::to_string(measurementCount);
    }
   
    return true;
}


void SenseairI2CSensor::DisableABC() {
    _abcEnabled = false;
}

void SenseairI2CSensor::ManualCalibration(int pBaseLinePPM) {

}

void SenseairI2CSensor::EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) {
    _abcEnabled = true;
}

void SenseairI2CSensor::ReadFirmwareVersion() {
    uint8_t version[2];
    if(ReadRegister((uint8_t )SENSEAIR_I2C_RERISTERS::FIRMWARE_REV,version, sizeof(version))) {
        _softVer = std::to_string(version[0])+"."+std::to_string(version[1]);       
    }
}

void SenseairI2CSensor::ReadSensorID() {
uint8_t sensorId[4];
    if(ReadRegister((uint8_t )SENSEAIR_I2C_RERISTERS::SENSOR_ID,sensorId, sizeof(sensorId))) {
        char buf[10];
        snprintf(buf, sizeof(buf), "%.2X%.2X%.2X%.2X", sensorId[0],sensorId[1], sensorId[2],sensorId[3]);
        _serialNo = buf;
    }
}

void SenseairI2CSensor::ReadCalibrationStatus() {
 
}


void SenseairI2CSensor::ReadMeasurementModeInfo() {
    uint8_t values[17];
    if(ReadRegister((uint8_t )SENSEAIR_I2C_RERISTERS::MEASUREMENT_MODE,values, sizeof(values))) {
        _valSingleMeasurementMode.value.b = values[0] == 1;
        _valMeasurementPeriod.value.i = values[1]*256 + values[2];
        _valMeasurementNoSamples.value.i = values[3]*256 + values[4];
        _valAbcPeriod.value.i = values[5]*256 + values[6];
        _valAbcTarget.value.i = values[9]*256 + values[10];
        _valFilter.value.i = values[12];
        auto meterControl = values[16];
        _meterControl = "";

        if(meterControl & (int)MeterControlModes::nRDYEnabled) _meterControl+="nRDY ";
        if(meterControl & (int)MeterControlModes::ABCEnabled) _meterControl+="ABC ";
        if(meterControl & (int)MeterControlModes::StaticIREnabled) _meterControl+="StaticIR ";
        if(meterControl & (int)MeterControlModes::DynmaicIREnabled) _meterControl+="DynmaicIR ";
        if(meterControl & (int)MeterControlModes::PressureCompensationEnabled) _meterControl+="PressureCompensation ";
        if(meterControl & (int)MeterControlModes::nRDNYPinInvert) _meterControl+="nRDNYPinInvert ";

    }
}

#define TAG "SenseI2C"



bool SenseairI2CSensor::ReadRegister(uint8_t pRegAddr, uint8_t *pRegData, uint32_t pLen)
{
 printf("Read(len=%d, %d)\n", pLen, (int)pRegAddr);

    esp_err_t ret = ESP_OK;
    
    
    for(auto attempts = 0; attempts < 5; attempts++) {
        if(attempts>0)
            vTaskDelay(500 / portTICK_RATE_MS);
        i2c_cmd_handle_t cmd = NULL;
        
        esp_err_t espRc =1;
        uint8_t cmdMem[I2C_LINK_RECOMMENDED_SIZE(1)] = { 0 };

        cmd = i2c_cmd_link_create_static(cmdMem, I2C_LINK_RECOMMENDED_SIZE(1));
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, 0x68 << 1 | I2C_MASTER_WRITE, 1 /* expect ack */);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100/portTICK_PERIOD_MS);
        i2c_cmd_link_delete_static(cmd);
           

        vTaskDelay(5 / portTICK_RATE_MS);

        cmd = i2c_cmd_link_create_static(cmdMem, I2C_LINK_RECOMMENDED_SIZE(1));
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, 0x68 << 1 | I2C_MASTER_WRITE, 1 /* expect ack */);
        i2c_master_write(cmd,&pRegAddr,1,true);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100/portTICK_PERIOD_MS);
        i2c_cmd_link_delete_static(cmd);

        ret = i2c_master_read_from_device(I2C_NUM_0, 0x68, pRegData, pLen, 1000 / portTICK_RATE_MS);

        if ( ret != ESP_OK ) {
            ESP_LOGE(TAG, "Fail to read device ( port = %d, addr = %02x, err=%d )", I2C_NUM_0, pRegAddr, ret);      
        } else 
            break;        
    }

    return ret == ESP_OK;

    return false;
}

bool SenseairI2CSensor::WriteRegister(uint8_t pRegAddr, const uint8_t *pRegData, uint32_t pLen)
{
    I2CDeviceSessionCommand commands[5] = {
        I2CDeviceSessionCommand(I2CDeviceSessionCommandType::Wakeup),
        I2CDeviceSessionCommand(I2CDeviceSessionCommandType::StartWrite),
        I2CDeviceSessionCommand(I2CDeviceSessionCommandType::WriteByte, pRegAddr),
        I2CDeviceSessionCommand(I2CDeviceSessionCommandType::WriteBytes, (uint8_t *)pRegData, pLen),
        I2CDeviceSessionCommand(I2CDeviceSessionCommandType::Execute)
    };
    auto result = _session->RunCommand(commands,5);
    if(result) printf("Sucessfully written\n");
	return result;
}