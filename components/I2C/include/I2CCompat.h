#include "Common.h"
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0)    
esp_err_t i2c_master_read_from_device(i2c_port_t i2c_num, uint8_t device_address,
                                      uint8_t* read_buffer, size_t read_size,
                                      TickType_t ticks_to_wait);
#endif