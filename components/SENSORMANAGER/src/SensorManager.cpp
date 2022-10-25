#include "SensorManager.h"

#include "MhZ19Sensor.h"
#include "CubicCO2Sensor.h"
#include "SenseairI2CSensor.h"
#include "SenseairModBusSensor.h"
#include "SCD30Sensor.h"

// To be abstracted further....
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)  
    #define SENSOR_ENABLE_GPIO (gpio_num_t)GPIO_NUM_19
#else
    #define SENSOR_ENABLE_GPIO (gpio_num_t)10
#endif


#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
#include "HWSerial.h"
#define SerialESP32 new HWSerial(CONFIG_CO2_UART_NUM)
#define Serial SerialESP32
#else 
#include "SoftSerial.h"
#define SerialESP8266 new SoftSerial(GPIO_NUM_15, GPIO_NUM_13);
#define Serial SerialESP8266
#endif

SensorManager::SensorManager(GeneralSettings& pSettings, I2C& pI2C, DataManager& pDataManager) : 
    _settings(pSettings), 
    _i2c(pI2C), 
    _dataManager(pDataManager) {
    ESP_ERROR_CHECK(gpio_set_direction(SENSOR_ENABLE_GPIO, (gpio_mode_t)GPIO_MODE_DEF_OUTPUT));
    EnableSensorReadGPIO();
    vTaskDelay(50 / portTICK_PERIOD_MS);        
    pI2C.Scan();
    auto count = _i2c.GetNumDevices();
    printf("Searching for I2C CO2 Sensor from %d devices: ", count);
    for(auto i = 0; i < count; i++) {
        auto device = _i2c.GetDevice(i);
        
        switch(device) {

            case 0x68 :
                _co2Sensor = new SenseairI2CSensor(_i2c.CreateSession(device), this);
                break;
            case 0x61 :
                _co2Sensor = new Scd30Sensor(_i2c.CreateSession(device));
                break;                
        }

        printf("%.2x ", device);
    }
    printf("\n");

    if(!_co2Sensor) {
        
        switch(_settings.GetCO2SensorType()) {
            case CO2SensorType::None :
                break;
            case CO2SensorType::MHZ19 :
                _serial = Serial;
                _co2Sensor = new MhZ19Sensor(_serial);
                break;
            case CO2SensorType::Cubic :
                _serial = Serial;
                _co2Sensor = new CubicCO2Sensor(_serial);
                break;
            case CO2SensorType::SenseAirModBus:
                _serial = Serial;
                _co2Sensor = new SenseairModBusSensor(_serial);
                break;
            default:
                break;
        }
    }
    _values.CO2 = _co2Sensor;
    _bme = new BME280(_i2c);
    DisableSensorReadGPIO();

}

SensorManager::~SensorManager() {

}

void SensorManager::EnableSensorReadGPIO()  {
    ESP_ERROR_CHECK(gpio_set_level(SENSOR_ENABLE_GPIO,1));
}

void SensorManager::DisableSensorReadGPIO() {
    ESP_ERROR_CHECK(gpio_set_level(SENSOR_ENABLE_GPIO,0));
}


ValueModel& SensorManager::GetValues() {
    return _values;
}


time_t SensorManager::UpdateValues() {
    time_t curTime;
    time(&curTime);
    if(curTime - _lastSensorRead >= _settings.GetSensorUpdateInterval()) {
      
        
        EnableSensorReadGPIO();
        if(_co2Sensor) _co2Sensor->RefreshValues();
        
        _bme->ReadSensorValues(_values.Bme280);
        DisableSensorReadGPIO();
        _lastSensorRead = curTime;
        time(&curTime);

        
        auto co2 = _co2Sensor ? (uint16_t)(_co2Sensor->GetPPM()) : (uint16_t)0;
        auto temp = (int16_t)(_values.Bme280.temperature*100);
        auto humidity = (uint16_t)(_values.Bme280.humidity*100);
        auto pressure = (uint)(_values.Bme280.pressure)- 80000;
       
        //auto pressure = (uint16_t)((_values.Bme280.pressure-800)*100);

        DataEntry entry = {
            .TimeStamp = curTime,
            .Temp = temp,
            .CO2 = co2,
            .Humidity = humidity, 
            .Pressure = (uint16_t)pressure
        };

        _dataManager.WriteEntry(entry);
        time(&curTime);
       
    }   
    return _settings.GetSensorUpdateInterval() - (curTime - _lastSensorRead); 
}

void SensorManager::Calibrate() {
    if(_co2Sensor==nullptr)
        return;

    _co2Sensor->ManualCalibration(400);
}