#include "SenseairI2CSensor.h"
#include "I2CDeviceSessionCommand.h"

//
// Borrowed from https://github.com/Niclas73/Sunrise-master/blob/master/src/sunrise_i2c.h 
//
enum SENSEAIR_I2C_RERISTERS
{
      ERROR_STATUS                        = 0x00, 
      CO2_FILTERED_COMPENSATED            = 0x06, 
      CHIP_TETMP                          = 0x08,
      MEASUREMENT_COUNT                   = 0x0D,
      MEASUREMENT_CYCLE_TIME              = 0x0E,
      CO2_UNFILTERED_COMPENSATED          = 0x10,
      CO2_FILTERED_UNCOMPENSATED          = 0x12,
      CO2_UNFILTERED_UNCOMPENSATED        = 0x14,
      FIRMWARE_TYPE                       = 0x2F, 
      FIRMWARE_REV                        = 0x38,
      SENSOR_ID                           = 0x3A,
      CALIBRATION_STATUS                  = 0x81,
      CALIBRATION_COMMAND                 = 0x82,
      CALIBRATION_TARGET                  = 0x84,
      ABC_TIME                            = 0x88, 
      ABC_0                               = 0x8A, 
      ABC_1                               = 0x8C,
      ABC_2                               = 0x8E, 
      ABC_3                               = 0x90,
      START_SINGLE_MEASUREMENT            = 0xC3,
      MEASUREMENT_MODE                    = 0x95, 
      MEASUREMENT_PERIOD                  = 0x96, 
      NBR_SAMPLES                         = 0x98,
      ABC_PERIOD                          = 0x9A, 
      CLEAR_ERROR_STATUS                  = 0x9D,
      ABC_TARGET                          = 0x9E, 
      IIR_FILTER_PARMETER                 = 0xA1,
      RESET_SENSOR                        = 0xA3,
      METER_CONTROL                       = 0xA5,
      I2C_ADDRESS                         = 0xA7,
      POWER_DOWN_DATA                     = 0xC4,
};


SenseairI2CSensor::SenseairI2CSensor(I2CDeviceSession* pSession, ISensorManager* pSensorManager) : _session(pSession), _sensorManager(pSensorManager) {
    _deviceName = "Senseair I2C";
    ReadFirmwareVersion();
    ReadSensorID();
    ReadCalibrationStatus();

    
    ReadMeasurementModeInfo();
    if(!_valSingleMeasurementMode.value.b) {
        uint8_t mode = 1;
        WriteRegister((uint8_t)SENSEAIR_I2C_RERISTERS::MEASUREMENT_MODE, &mode, sizeof(mode));
        ReadMeasurementModeInfo();
    }
}

SenseairI2CSensor::~SenseairI2CSensor() {
    delete _session;
}





bool SenseairI2CSensor::RefreshValues() {
    uint8_t ppmBytes[0xf];
    uint8_t errorStatus[2];
    vTaskDelay(50 / portTICK_RATE_MS);      

    if(_valSingleMeasurementMode.value.b) {
        if(_hasDataRestore) 
            WriteRegister((uint8_t)SENSEAIR_I2C_RERISTERS::START_SINGLE_MEASUREMENT, _sensorStateData, sizeof(_sensorStateData));
        else 
            WriteRegister((uint8_t)SENSEAIR_I2C_RERISTERS::START_SINGLE_MEASUREMENT, _sensorStateData, 1);
            
        vTaskDelay(2450 / portTICK_RATE_MS);      
        if(ReadRegister((uint8_t )SENSEAIR_I2C_RERISTERS::CO2_FILTERED_COMPENSATED,ppmBytes, sizeof(ppmBytes))) {
                _ppm = ppmBytes[0]*256 + ppmBytes[1];
                ushort tempRaw = ppmBytes[2]*256 + ppmBytes[3];
                ushort measurementCount = ppmBytes[7];
                ushort measurementSeconds = ppmBytes[8]*256 + ppmBytes[9];
                measurementSeconds*=2;
            _temp = std::to_string(tempRaw/100)+"."+std::to_string(tempRaw % 100);

            _measurementInfo = std::to_string(measurementSeconds) + "s since reading #"+std::to_string(measurementCount);
        }

        if(ReadRegister((uint8_t)SENSEAIR_I2C_RERISTERS::POWER_DOWN_DATA, _sensorStateData+1, sizeof(_sensorStateData)-1)) 
            _hasDataRestore = true;
    } else {
        if(ReadRegister((uint8_t )SENSEAIR_I2C_RERISTERS::CO2_FILTERED_COMPENSATED,ppmBytes, sizeof(ppmBytes))) {
                _ppm = ppmBytes[0]*256 + ppmBytes[1];
                ushort tempRaw = ppmBytes[2]*256 + ppmBytes[3];
                ushort measurementCount = ppmBytes[7];
                ushort measurementSeconds = ppmBytes[8]*256 + ppmBytes[9];
                measurementSeconds*=2;
            _temp = std::to_string(tempRaw/100)+"."+std::to_string(tempRaw % 100);

            _measurementInfo = std::to_string(measurementSeconds) + "s since reading #"+std::to_string(measurementCount);
        }
    }
   
    return true;
}


bool SenseairI2CSensor::WriteCalibrationStatus(uint8_t pStatus) {
    return WriteRegister((uint8_t)SENSEAIR_I2C_RERISTERS::CALIBRATION_STATUS, (const uint8_t*)&pStatus, sizeof(pStatus));
}


bool SenseairI2CSensor::WriteCalibrationCommand(uint8_t pByte1, uint8_t pByte2) {
    uint8_t cmdBuf[2];
    cmdBuf[0] = pByte1;
    cmdBuf[1] = pByte2;

    return WriteRegister((uint8_t)SENSEAIR_I2C_RERISTERS::CALIBRATION_COMMAND, (const uint8_t*)&cmdBuf, sizeof(cmdBuf));
}


void SenseairI2CSensor::DisableABC() {
    _abcEnabled = false;
}

void SenseairI2CSensor::ManualCalibration(int pBaseLinePPM) {
    _sensorManager->EnableSensorReadGPIO();
    if(!WriteCalibrationStatus(0x00)) {
        printf("Failed to set calbration status\n");
        return;
    }

    if(!WriteCalibrationCommand(0x7c, 0x06)) {
        printf("Failed to send calibration command\n");
    }    
    if(_hasDataRestore) 
        WriteRegister((uint8_t)SENSEAIR_I2C_RERISTERS::START_SINGLE_MEASUREMENT, _sensorStateData, sizeof(_sensorStateData));
    else 
        WriteRegister((uint8_t)SENSEAIR_I2C_RERISTERS::START_SINGLE_MEASUREMENT, _sensorStateData, 1);
        
    vTaskDelay(2450 / portTICK_RATE_MS);      


    uint8_t calibrationStatus = 0;
    if(ReadRegister((uint8_t)SENSEAIR_I2C_RERISTERS::CALIBRATION_STATUS, &calibrationStatus, sizeof(calibrationStatus))) 
        _hasDataRestore = true;

    if(ReadRegister((uint8_t)SENSEAIR_I2C_RERISTERS::POWER_DOWN_DATA, _sensorStateData+1, sizeof(_sensorStateData)-1)) 
        _hasDataRestore = true;
    _sensorManager->DisableSensorReadGPIO();
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
    esp_err_t ret = ESP_OK;
    
    
    for(auto attempts = 0; attempts < 5; attempts++) {
        if(attempts>0)
            vTaskDelay(500 / portTICK_RATE_MS);

        
        esp_err_t espRc =1;
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
        uint8_t cmdMem[I2C_LINK_RECOMMENDED_SIZE(1)] = { 0 };
        auto cmd = i2c_cmd_link_create_static(cmdMem, I2C_LINK_RECOMMENDED_SIZE(1));
        #else
        auto cmd = i2c_cmd_link_create();
        #endif
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, 0x68 << 1 | I2C_MASTER_WRITE, 1 /* expect ack */);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100/portTICK_PERIOD_MS);
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
        i2c_cmd_link_delete_static(cmd);
        #else
        i2c_cmd_link_delete(cmd);
        #endif
           

        vTaskDelay(5 / portTICK_RATE_MS);

        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
        cmd = i2c_cmd_link_create_static(cmdMem, I2C_LINK_RECOMMENDED_SIZE(1));
        #else
        cmd = i2c_cmd_link_create();
        #endif
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, 0x68 << 1 | I2C_MASTER_WRITE, 1 /* expect ack */);
        i2c_master_write(cmd,&pRegAddr,1,true);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100/portTICK_PERIOD_MS);
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
        i2c_cmd_link_delete_static(cmd);
        #else
        i2c_cmd_link_delete(cmd);
        #endif

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
    printf("Write(len=%d, %d)\n", pLen, (int)pRegAddr);

    esp_err_t ret = ESP_OK;
    
    
    for(auto attempts = 0; attempts < 5; attempts++) {
        if(attempts>0)
            vTaskDelay(500 / portTICK_RATE_MS);

        
        esp_err_t espRc =1;
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
        uint8_t cmdMem[I2C_LINK_RECOMMENDED_SIZE(1)] = { 0 };
        auto cmd = i2c_cmd_link_create_static(cmdMem, I2C_LINK_RECOMMENDED_SIZE(1));
        #else
        auto cmd = i2c_cmd_link_create();
        #endif
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, 0x68 << 1 | I2C_MASTER_WRITE, 1 /* expect ack */);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100/portTICK_PERIOD_MS);
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
        i2c_cmd_link_delete_static(cmd);
        #else
        i2c_cmd_link_delete(cmd);
        #endif
           

        vTaskDelay(5 / portTICK_RATE_MS);

        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
        cmd = i2c_cmd_link_create_static(cmdMem, I2C_LINK_RECOMMENDED_SIZE(1));
        #else
        cmd = i2c_cmd_link_create();
        #endif
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, 0x68 << 1 | I2C_MASTER_WRITE, 1 /* expect ack */);
        i2c_master_write(cmd,&pRegAddr,1,true);
        i2c_master_write(cmd,pRegData,pLen,true);
        i2c_master_stop(cmd);
        ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100/portTICK_PERIOD_MS);
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
        i2c_cmd_link_delete_static(cmd);
        #else
        i2c_cmd_link_delete(cmd);
        #endif
        vTaskDelay(25 / portTICK_RATE_MS);
       
        if ( ret != ESP_OK ) {
            ESP_LOGE(TAG, "Fail to write device ( port = %d, addr = %02x, err=%d )", I2C_NUM_0, pRegAddr, ret);      
        } else 
            break;        
    }

    return ret == ESP_OK;

    return false;
}


int SenseairI2CSensor::GetRequiredManualCalibrationWaitInSeconds() {
    return 2;
}