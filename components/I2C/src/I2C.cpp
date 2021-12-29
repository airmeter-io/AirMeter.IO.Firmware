#include "I2C.h"

I2C::I2C(gpio_num_t pSdaPin, gpio_num_t pSclPin) {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)

#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          100000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

	int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = CONFIG_I2C_SDA_PIN,
        .scl_io_num =CONFIG_I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master = {.clk_speed = I2C_MASTER_FREQ_HZ },
		.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
    };


    i2c_param_config(i2c_master_port, &conf);

    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
#else 
    i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = pSdaPin;
	conf.scl_io_num = pSclPin;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.clk_stretch_tick = 300;

    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER);
	i2c_param_config(I2C_NUM_0, &conf);
#endif
}

bool I2C::Scan() {
	uint8_t temp[0x78];
    int i;
	esp_err_t espRc;
    int index = 0;
	for (i=3; i< 0x78; i++) {
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, 1 /* expect ack */);
		i2c_master_stop(cmd);

		espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 25/portTICK_PERIOD_MS);
		if (espRc != 0xffffffff) 
        {
            temp[index] = i;
            index++;            
        }
		i2c_cmd_link_delete(cmd);
	}
	_numDevices = index;
	if(_devices!=nullptr) free(_devices);
	_devices = (uint8_t *)malloc(_numDevices);
	memcpy(_devices, temp, _numDevices);
    return true;
}

size_t I2C::GetNumDevices() {
	return _numDevices;
}

uint8_t I2C::GetDevice(uint pIndex) {
	if(pIndex>= _numDevices) return 0;

	return _devices[pIndex];
}

i2c_port_t I2C::GetI2CPort() {
    return I2C_NUM_0;
}

I2CDeviceSession* I2C::CreateSession(uint8_t pDeviceId) {
	return new I2CDeviceSession(I2C_NUM_0, pDeviceId);
}
    
I2C::~I2C() {
	if(_devices!=nullptr) free(_devices);
}