#include "I2CCompat.h"
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0)    
esp_err_t i2c_master_read_from_device(i2c_port_t i2c_num, uint8_t device_address,
                                      uint8_t* read_buffer, size_t read_size,
                                      TickType_t ticks_to_wait)
{
    esp_err_t err = ESP_OK;


    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    assert (handle != NULL);

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_write_byte(handle, device_address << 1 | I2C_MASTER_READ, true);
    if (err != ESP_OK) {
        goto end;
    }

    err = i2c_master_read(handle, read_buffer, read_size, I2C_MASTER_LAST_NACK);
    if (err != ESP_OK) {
        goto end;
    }

    i2c_master_stop(handle);
    err = i2c_master_cmd_begin(i2c_num, handle, ticks_to_wait);

end:
    i2c_cmd_link_delete(handle);
    return err;
}
#endif
