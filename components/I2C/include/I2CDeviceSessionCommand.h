#pragma once

#include "Common.h"
#include "I2C.h"
#include "I2CDeviceSession.h"

#define TAGI2C "I2CDeviceSession"

enum I2CDeviceSessionCommandType { 
    StartWrite,
    StartRead,
    WriteByte,
    WriteBytes,    
    Execute,
    Wakeup
};

class I2CDeviceSessionCommand {

public:
    I2CDeviceSessionCommand(I2CDeviceSessionCommandType pType);
    I2CDeviceSessionCommand(I2CDeviceSessionCommandType pType, uint8_t pByte);
    I2CDeviceSessionCommand(I2CDeviceSessionCommandType pType, uint8_t* pBytes, uint pLength);
    I2CDeviceSessionCommand(I2CDeviceSessionCommandType pType,uint pDelayInMs, bool pQuiet );
    I2CDeviceSessionCommandType Type;
    union {
        struct {
            uint8_t Byte;
        } SingleByte;
        struct {
            uint8_t* Bytes;
            uint Len;
        } MultipleBytes;
        struct {
            uint DelayInMs;
            bool Quiet;  
        } Execute;
    } Data;
};

// inline bool I2CDeviceSessionCommand::StartWriteCommand() {
//     auto result = i2c_master_start(_cmd);
//     if(result) {
//          ESP_LOGE(TAGI2C, "StartWriteCommand.i2c_master_start ( port = %d, dev = %02x, err=%d)", _session._port,  _session._deviceId, result);
//          return false;
//     }
//     result = i2c_master_write_byte(_cmd, _session._deviceId << 1 | I2C_MASTER_WRITE, true);
//     if(result) {
//          ESP_LOGE(TAGI2C, "StartWriteCommand.i2c_master_write_byte ( port = %d, dev = %02x, err=%d)", _session._port,  _session._deviceId, result);
//          return false;
//     }
//     //printf("started write\n");
//     return true;
// }

// inline bool I2CDeviceSessionCommand::StartReadCommand(uint8_t* pBytes, uint pLength) {
//     auto result = i2c_master_start(_cmd);
//     if(result) {
//          ESP_LOGE(TAGI2C, "StartReadCommand.i2c_master_start ( port = %d, dev = %02x, err=%d)", _session._port,  _session._deviceId, result);
//          return false;
//     }
//     result = i2c_master_write_byte(_cmd, _session._deviceId << 1 | I2C_MASTER_READ, true);
//     if(result) {
//          ESP_LOGE(TAGI2C, "StartReadCommand.i2c_master_write_byte ( port = %d, dev = %02x, err=%d)", _session._port,  _session._deviceId, result);
//          return false;
//     }
//     if(pBytes!=nullptr) {
//         result = i2c_master_read(_cmd,pBytes, pLength, I2C_MASTER_LAST_NACK);
//         if(result) {
//             ESP_LOGE(TAGI2C, "StartReadCommand.i2c_master_read ( port = %d, dev = %02x, err=%d)", _session._port,  _session._deviceId, result);
//             return false;
//         }
//     }

//     return true;
// }

// inline bool I2CDeviceSessionCommand::WriteByte(uint8_t pByte) {
//     auto result = i2c_master_write_byte(_cmd,pByte,true);
//     if(result) {
//          ESP_LOGE(TAGI2C, "StartReadCommand.i2c_master_write_byte ( port = %d, dev = %02x, err=%d)", _session._port,  _session._deviceId, result);
//          return false;
//     }
//     return true;
// }
 
// inline bool I2CDeviceSessionCommand::WriteBytes(const uint8_t* pBytes, uint pLength) {
//     auto result = i2c_master_write(_cmd, (uint8_t *)pBytes, pLength, I2C_MASTER_LAST_NACK);
//     if(result) {
//          ESP_LOGE(TAGI2C, "StartReadCommand.i2c_master_write_byte ( port = %d, dev = %02x, err=%d)", _session._port,  _session._deviceId, result);
//          return false;
//     }
//     return true;
// }

// inline bool I2CDeviceSessionCommand::ExecuteCommand(uint pDelayInMs, bool pQuiet) {
//     i2c_master_stop(_cmd);

//     auto result = i2c_master_cmd_begin(_session._port, _cmd, pDelayInMs/portTICK_PERIOD_MS);
//     if(result && !pQuiet) {
//          ESP_LOGE(TAGI2C, "ExecuteCommand ( port = %d, dev = %02x, err=%d)", _session._port,  _session._deviceId, result);
//     }

//     return result == 0; 
// }