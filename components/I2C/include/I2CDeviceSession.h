#pragma once

#include "Common.h"


class I2CDeviceSessionCommand;
class I2CDeviceSession {
        i2c_port_t _port;
        uint8_t _deviceId;
        uint8_t _bufIndex = 0;
        friend class I2CDeviceSessionCommand;
    public:
        I2CDeviceSession(i2c_port_t pPort, uint8_t pDeviceId);
        ~I2CDeviceSession();

        bool RunCommand(I2CDeviceSessionCommand* pCommands, uint pNumCommands);

};