#include "BME280.h"
#include "CommonValueNames.h"
#include<math.h>

void user_delay_ms(uint32_t period, void *pUserInfo)
{
	vTaskDelay(period / 1000);
}

int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ((BME280 *)intf_ptr)->GetDeviceId() << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(((BME280 *)intf_ptr)->GetI2C().GetI2CPort(), cmd, 1000 / portTICK_PERIOD_MS);
    if(ret!=0) printf("I2C ReadPreWrite: %s: %d\n",ret<0 ? "ERROR" : "WARNING", ret);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ((BME280 *)intf_ptr)->GetDeviceId() << 1 | I2C_MASTER_READ, true);
    i2c_master_read(cmd, reg_data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(((BME280 *)intf_ptr)->GetI2C().GetI2CPort(), cmd, 1000 / portTICK_PERIOD_MS);
    if(ret!=0) printf("I2C Read: %s: %d\n",ret<0 ? "ERROR" : "WARNING", ret);
    i2c_cmd_link_delete(cmd);
    
	return 0;
}

int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    
    i2c_master_write_byte(cmd, ((BME280 *)intf_ptr)->GetDeviceId() << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write(cmd, (uint8_t *)reg_data, len, true);
    i2c_master_stop(cmd);
    auto result = i2c_master_cmd_begin(((BME280 *)intf_ptr)->GetI2C().GetI2CPort(), cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if(result!=0) printf("I2C Write: %s: %d\n",result<0 ? "ERROR" : "WARNING", result);
	return 0;
}



I2C& BME280::GetI2C() const {
    return _i2c;
}

BME280::BME280(I2C& pI2C) : _i2c(pI2C) {
    AddValueSource(new ValueSource(*this,TEMPERATURE,   AtmosphericTemperature, Fixed, Centigrade, _valHumidity, GET_LATEST_DATA));       
    AddValueSource(new ValueSource(*this,PRESSURE,      AtmosphericPressure,    Fixed, hPa,        _valPressure, GET_LATEST_DATA));       
    AddValueSource(new ValueSource(*this,HUMIDITY,      AtmosphericHumidity,    Fixed, Percent,    _valTemperature, GET_LATEST_DATA));    
    _bme.chip_id = BME280_I2C_ADDR_PRIM;
	_bme.intf = BME280_I2C_INTF;
	_bme.read = user_i2c_read;
	_bme.write = user_i2c_write;
	_bme.delay_us = user_delay_ms;
    _bme.intf_ptr = (void *)this;
    
    int8_t result = bme280_init(&_bme);
    if(result!=0) printf("%s: %d\n",result<0 ? "ERROR" : "WARNING", result); else printf("Successfully initiatised BME280\n");


    _bme.settings.osr_h = BME280_OVERSAMPLING_1X;
	_bme.settings.osr_p = BME280_OVERSAMPLING_16X;
	_bme.settings.osr_t = BME280_OVERSAMPLING_2X;
	_bme.settings.filter = BME280_FILTER_COEFF_16;
	_bme.settings.standby_time = BME280_STANDBY_TIME_62_5_MS;
    
    uint8_t settings_sel;
	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	result = bme280_set_sensor_settings(settings_sel, &_bme);
	result = bme280_set_sensor_mode(BME280_NORMAL_MODE, &_bme);

}

const std::string& BME280::GetValuesSourceName() const {
    return SOURCE_NAME;
}


bool BME280::ReadSensorValues() {
    struct bme280_data comp_data;
    auto result = bme280_get_sensor_data(BME280_ALL, &comp_data, &_bme);
    if(result!=0) {
        printf("ReadBME280 - %s: %d\n",result<0 ? "ERROR" : "WARNING", result);
        if(result<0)
            return false;
    }

    _valTemperature.i= round(comp_data.temperature*100);
    _valPressure.i = round(comp_data.pressure);
    _valHumidity.i = round(comp_data.humidity*100);
    return true;
}

uint8_t BME280::GetDeviceId() const {
    return BME280_I2C_ADDR_PRIM;
}

BME280::~BME280() {

}

