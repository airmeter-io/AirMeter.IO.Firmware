#include "SCD30Sensor.h"
#define TAG "SCD30"
#include<string>
#include "I2CCompat.h"


const std::string GROUP_SCD30 = "Scd30";

const ValueIdentifier SCD30_TEMPERATURE         = { .Id = 1000, .Grouping = GROUP_SCD30, .Name = "Temperature" };
const ValueIdentifier SCD30_HUMIDITY            = { .Id = 1001, .Grouping = GROUP_SCD30, .Name = "Humidity"} ;
const ValueIdentifier SCD30_MEASUREMENTINTERVAL = { .Id = 1002, .Grouping = GROUP_SCD30, .Name = "MeasurementInterval"};
const ValueIdentifier SCD30_TEMPERATUREOFFSET   = { .Id = 1003, .Grouping = GROUP_SCD30, .Name = "TemperatureOffset"};
const ValueIdentifier SCD30_ALTITUDE            = { .Id = 1004, .Grouping = GROUP_SCD30, .Name = "Altitude"};

Scd30Sensor::Scd30Sensor(I2CDeviceSession* pSession) : _session(pSession) {    
    _deviceName = "SCD30 CO2 Sensor";

    AddValueSource(new ValueSource(*this,SCD30_TEMPERATURE,        Double, Dimensionless, _valTemperature, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SCD30_HUMIDITY,           Double, Dimensionless, _valHumidity, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SCD30_MEASUREMENTINTERVAL,Int,    Dimensionless, _valMeasurementInterval, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SCD30_TEMPERATUREOFFSET,  Int,    Dimensionless, _valTemperatureOffset, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SCD30_ALTITUDE,           Int,    Dimensionless, _valAltitude, GET_LATEST_DATA));
    
    _valMaxPPM.i = 5000;
    _valBasePPM.i = 400;
    _valMinBasePPM.i = 400;
    _valMaxBasePPM.i = 2000;
    _valHoursPerAbcCycle.i = 7*24;
    _valMaxHoursPerAbcCycle.i = 7*24;
    _valMinHoursPerAbcCycle.i = 7*24;

    ReadSoftwareVersion();    
    ReadSerialNo();
    ReadMeasurementInterval();      
    ReadABCStatus();  
    ReadTemperatureOffset();  
    ReadAltitude();

}


const std::string& Scd30Sensor::GetValuesSourceName() const {
   return SOURCE_NAME;
}


uint Scd30Sensor::Crc8(void *pData, size_t pLength)
{
    auto *data = (unsigned char const *)pData;
    uint8_t result = 0xff;
    for (auto i = 0; i < pLength; ++i)
    {
        result ^= data[i];
        for (auto iter = 0; iter < 8; iter++)
            result = result & 0x80 ? (result << 1) ^ 0x31 :  (result << 1);
    }
    return result;
}

bool Scd30Sensor::ReadCommand(Scd30I2CCommand pCommand, uint8_t* pParamData, uint pNumParamBytes, void * pResponseData, uint pNumResponseBytes) {
    auto numParamWords = pNumParamBytes % 2 ? pNumParamBytes/2 + 1 : pNumParamBytes/2;
    uint8_t buffer[2 + (numParamWords * 3)];

    buffer[0] = pCommand >> 8;
    buffer[1] = pCommand & 0xff;
    auto ptr = buffer+2;
   
    if (pParamData && numParamWords)
        for (size_t i = 0; i < numParamWords; i++)
        {
            *ptr = pParamData[i*2];
            *(ptr+1) = pParamData[i*2+1];
            *(ptr + 2) = Crc8(ptr, 2);
            ptr+=3;
        }

    ESP_LOGV(TAG, "Sending buffer:");
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    uint8_t cmdMem[I2C_LINK_RECOMMENDED_SIZE(1)] = { 0 };
    auto cmd = i2c_cmd_link_create_static(cmdMem, I2C_LINK_RECOMMENDED_SIZE(1));
    #else
    auto cmd = i2c_cmd_link_create();
    #endif

    printf("Sending %.4x: ", pCommand);
    for(auto i = 0; i <sizeof(buffer) ; i++)
        printf("%.2x ",buffer[i]);            
    printf("\n");
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0x61 << 1 | I2C_MASTER_WRITE, 1);
    i2c_master_write(cmd,buffer,sizeof(buffer),true);
    i2c_master_stop(cmd);
    auto ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 100/portTICK_PERIOD_MS);

    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)    
    i2c_cmd_link_delete_static(cmd);
    #else
    i2c_cmd_link_delete(cmd);
    #endif
    ets_delay_us(5500);
    auto numResponseWords = pNumResponseBytes % 2 ? pNumResponseBytes/2 + 1 : pNumResponseBytes/2;
    if(pResponseData && numResponseWords) {
        uint8_t inBuf[3*numResponseWords];
        uint8_t *ptr = inBuf;
        memset(inBuf, 0, sizeof(inBuf));
        ret = i2c_master_read_from_device(I2C_NUM_0, 0x61, inBuf, sizeof(inBuf), 1000 / portTICK_PERIOD_MS);        
        if(ret!= ESP_OK) {
            printf("Error reading %d\n", (int)ret);
            return false;
        } else {
            auto responseData = (uint8_t *)pResponseData;
            for (size_t i = 0; i < numResponseWords; i++)
            {                
                uint8_t crc = Crc8(ptr, 2);
                if (crc != *(ptr + 2))
                {
                    ESP_LOGE(TAG, "Invalid CRC value expected 0x%02x, but was supplied 0x%02x", crc, *(ptr + 2));
                    return false;
                }
                responseData[i*2] = *ptr;
                responseData[i*2 +1] = *(ptr+1);   
                ptr+=3;             
            }
            printf("Received: ");
            for(auto i = 0; i <pNumResponseBytes ; i++)
                printf("%.2x ",responseData[i]);            
            printf("\n");
        }
    }

    return true;
}

bool Scd30Sensor::RefreshValues() {
    uint8_t valueData[12];
    uint8_t readyStatus[2] = {0,0};
    while(!ReadCommand(Scd30I2CCommand::GET_READY_STATUS, nullptr, 0, (uint16_t *)readyStatus, sizeof(readyStatus)) || readyStatus[1]!=1)
    {
        ESP_LOGE(TAG, "Not ready");
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }

    while(!ReadCommand(Scd30I2CCommand::READ_MEASUREMENT, nullptr, 0, (uint16_t *)valueData, sizeof(valueData)))
    {
        ESP_LOGE(TAG, "Failed to read measurement from SCD30");
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    uint32_t co2Raw =(uint32_t)((valueData[0] << 24) | (valueData[1] << 16) | (valueData[2] << 8) | valueData[3]  );
    uint32_t tempRaw =(uint32_t)((valueData[4] << 24) | (valueData[5] << 16) | (valueData[6] << 8) | valueData[7]  );
    uint32_t rhRaw =(uint32_t)((valueData[8] << 24) | (valueData[9] << 16) | (valueData[10] << 8) | valueData[11]  );
    float* co2 = ((float *)(&co2Raw));
    float* temp =((float *)(&tempRaw));
    float* rh = ((float *)(&rhRaw));
    _valCo2.i = *co2;
    _valCo2Unfiltered.i = *co2;
    _valCo2Uncompensated.i = *co2;
    _valCo2UnfilteredUncompensated.i = *co2;

    _valTemperature.d = *temp;
    _valHumidity.d = *rh;
    return true;
}

void Scd30Sensor::ReadSerialNo() {
    uint8_t serialWords[32];

    if(!ReadCommand(Scd30I2CCommand::READ_SERIAL_NUMBER, nullptr, 0, (uint16_t *)serialWords, sizeof(serialWords)))
    {
        ESP_LOGE(TAG, "Failed to read serial number from SCD30");
       // return;
    }
   
    _serialNo = "";
    for(auto i = 0; i < sizeof(serialWords) && serialWords[i]; i++) 
        _serialNo+=(char)serialWords[i];
    printf("Serisl: %s\n",  _serialNo.c_str());
}

void Scd30Sensor::ReadSoftwareVersion() {
    uint8_t fwVersion[2] = {0,0};
    while(!ReadCommand(Scd30I2CCommand::READ_FIRMWARE_REV, nullptr, 0, &fwVersion, 1))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGV(TAG, "Failed to read software version from SCD30");
        //return;
    }
    _softwareVersion = std::to_string((int)(fwVersion[0]))+"."+std::to_string((int)(fwVersion[1]));
     printf("Softver: %s\n",  _softwareVersion.c_str());
}

void Scd30Sensor::ReadMeasurementInterval() {
    uint8_t intervalBytes[2] = {0,0};
    while(!ReadCommand(Scd30I2CCommand::SET_MEASUREMENT_INTERVAL, nullptr, 0, &intervalBytes, 1))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGV(TAG, "Failed to read measurement interval from SCD30");
        return;
    }
    _valMeasurementInterval.i = (intervalBytes[0] << 8) + intervalBytes[1];
     printf("Measurement Interval: %d\n",  _valMeasurementInterval.i);
    if(_valMeasurementInterval.i!=2)  {
        SetMeasurementInterval(2);
        while(!ReadCommand(Scd30I2CCommand::SET_MEASUREMENT_INTERVAL, nullptr, 0, &intervalBytes, 1))
        {
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_LOGV(TAG, "Failed to read measurement interval from SCD30");
            return;
        }
        _valMeasurementInterval.i = (intervalBytes[0] << 8) + intervalBytes[1];
         printf("Measurement Interval (Reset): %d\n",  _valMeasurementInterval.i);
    }
   
}   

void Scd30Sensor::ReadABCStatus() {
    uint8_t bytes[2] = {0,0};
    while(!ReadCommand(Scd30I2CCommand::GET_OR_SET_ABC, nullptr, 0, &bytes, 1))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGV(TAG, "Failed to read ABC status from SCD30");
       // return;
    }
    printf("ABC %d\n",  ((bytes[0] << 8) + bytes[1]));
    _valIsAbcEnabled.b = ((bytes[0] << 8) + bytes[1])==1;    
}

void Scd30Sensor::ReadTemperatureOffset() {
    uint8_t bytes[2] = {0,0};
    while(!ReadCommand(Scd30I2CCommand::SET_TEMP_OFFSET, nullptr, 0, &bytes, 1))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGV(TAG, "Failed to temperature offset from SCD30");
        return;
    }
    _valTemperatureOffset.i = (bytes[0] << 8) + bytes[1];
}

void Scd30Sensor::ReadAltitude() {
    uint8_t bytes[2] = {0,0};
    while(!ReadCommand(Scd30I2CCommand::ALTITUDE_COMPENSATION, nullptr, 0, &bytes, 1))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGV(TAG, "Failed to read altitude from SCD30");
        return;
    }
    _valAltitude.i = (bytes[0] << 8) + bytes[1];
}


void Scd30Sensor::DisableABC() { 
    uint8_t paramBytes[2]  = {0,0};
    while(!ReadCommand(Scd30I2CCommand::GET_OR_SET_ABC, paramBytes, sizeof(paramBytes), nullptr, 0))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGV(TAG, "Failed to disable ABC on SCD30");
    }
    ReadABCStatus();  
}

void Scd30Sensor::ManualCalibration(int pBaseLinePPM) {
    uint8_t readyStatus[2] = {0,0};
    while(!ReadCommand(Scd30I2CCommand::GET_READY_STATUS, nullptr, 0, (uint16_t *)readyStatus, sizeof(readyStatus)) || readyStatus[1]!=1)
    {
        ESP_LOGE(TAG, "Not ready");
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    uint8_t paramBytes[2];
    paramBytes[0] = pBaseLinePPM >> 8;
    paramBytes[1] = pBaseLinePPM & 0xFF;

    while(!ReadCommand(Scd30I2CCommand::SET_EXTERNAL_REF_VALUE, paramBytes, sizeof(paramBytes), nullptr, 0))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGV(TAG, "Failed to set FRC value on SCD30");
    }
}

void Scd30Sensor::EnableABC(int pBaseLinePPM, int pNumberOfHoursPerCycle) { 
    uint8_t paramBytes[2]  = {0,1};
    while(!ReadCommand(Scd30I2CCommand::GET_OR_SET_ABC, paramBytes, sizeof(paramBytes), nullptr, 0))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGV(TAG, "Failed to enable ABC on SCD30");
    }
    ReadABCStatus();  
}

void Scd30Sensor::SetMeasurementInterval(uint16_t pIntervalSeconds) { 
    uint8_t paramBytes[2];
    paramBytes[0] = pIntervalSeconds >> 8;
    paramBytes[1] = pIntervalSeconds & 0xFF;
    while(!ReadCommand(Scd30I2CCommand::SET_MEASUREMENT_INTERVAL, paramBytes, sizeof(paramBytes), nullptr, 0))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGV(TAG, "Failed toset measurement interval on SCD30");
    }
    //ReadMeasurementInterval();
}