#include "I2CDeviceSession.h"
#include "I2CDeviceSessionCommand.h"

#define TAG "I2CDeviceSession"

I2CDeviceSession::I2CDeviceSession(i2c_port_t pPort, uint8_t pDeviceId) : _port(pPort), _deviceId(pDeviceId) {

}

I2CDeviceSession::~I2CDeviceSession() {

}

bool I2CDeviceSession::RunCommand(I2CDeviceSessionCommand* pCommands, uint pNumCommands) {
    esp_err_t result = 0;
     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    for(auto i = 0; i < pNumCommands; i++)
        switch(pCommands[i].Type) {
            case I2CDeviceSessionCommandType::StartWrite :
                result = i2c_master_start(cmd);
                if(result) {
                    ESP_LOGE(TAG, "StartReadCommand.i2c_master_start (cmd=%d port = %d, dev = %02x, err=%d)",i, _port,  _deviceId, result);
                }
                result = i2c_master_write_byte(cmd,_deviceId << 1 | I2C_MASTER_WRITE, true);
                if(result) {
                    ESP_LOGE(TAG, "StartReadCommand.i2c_master_start (cmd=%d port = %d, dev = %02x, err=%d)",i, _port,  _deviceId, result);
                }
                break;
            case I2CDeviceSessionCommandType::StartRead :
                result = i2c_master_start(cmd);
                if(result) {
                    ESP_LOGE(TAG, "StartReadCommand.i2c_master_start (cmd=%d port = %d, dev = %02x, err=%d)",i, _port,  _deviceId, result);
                }
                result = i2c_master_write_byte(cmd, _deviceId << 1 | I2C_MASTER_READ, true);
                if(result) {
                    ESP_LOGE(TAGI2C, "StartReadCommand.i2c_master_write_byte ( port = %d, dev = %02x, err=%d)", _port,  _deviceId, result);
                }
                if(pCommands[i].Data.MultipleBytes.Bytes!=nullptr) {
                    result = i2c_master_read(cmd,pCommands[i].Data.MultipleBytes.Bytes, pCommands[i].Data.MultipleBytes.Len, I2C_MASTER_LAST_NACK);
                    if(result) {
                        ESP_LOGE(TAGI2C, "StartReadCommand.i2c_master_read ( port = %d, dev = %02x, err=%d)", _port,  _deviceId, result);
                    }
                }
                break;
            case I2CDeviceSessionCommandType::WriteByte :
                result = i2c_master_write_byte(cmd, _deviceId << 1 | I2C_MASTER_READ, true);
                if(result) {
                    ESP_LOGE(TAGI2C, "StartReadCommand.i2c_master_write_byte ( port = %d, dev = %02x, err=%d)", _port,  _deviceId, result);
                }
                break;
            case I2CDeviceSessionCommandType::WriteBytes :   
                result = i2c_master_write(cmd, pCommands[i].Data.MultipleBytes.Bytes, pCommands[i].Data.MultipleBytes.Len, I2C_MASTER_LAST_NACK);
                if(result) {
                    ESP_LOGE(TAGI2C, "StartReadCommand.i2c_master_write ( port = %d, dev = %02x, err=%d)", _port,  _deviceId, result);
                }
                break;
            case I2CDeviceSessionCommandType::Execute :
                i2c_master_stop(cmd);
                result = i2c_master_cmd_begin(_port, cmd, pCommands[i].Data.Execute.DelayInMs/portTICK_PERIOD_MS);
                if(result && !pCommands[i].Data.Execute.Quiet) {
                    ESP_LOGE(TAGI2C, "ExecuteCommand ( port = %d, dev = %02x, err=%d)", _port,  _deviceId, result);
                }
                break;
            case I2CDeviceSessionCommandType::Wakeup :
                 while(true) {
                    i2c_master_start(cmd);
                    i2c_master_write_byte(cmd, 0x68 << 1 | I2C_MASTER_READ, 1 /* expect ack */);
                  //  i2c_master_stop(cmd);
                  //  i2c_cmd_link_delete(cmd);
                    cmd = i2c_cmd_link_create();
                    if(!i2c_master_cmd_begin(I2C_NUM_0, cmd, 100/portTICK_PERIOD_MS))
                        break;        
                }
                break;
        }
    i2c_cmd_link_delete(cmd);
    return !result;
}




