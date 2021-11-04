#include "I2CDeviceSessionCommand.h"

I2CDeviceSessionCommand::I2CDeviceSessionCommand(I2CDeviceSessionCommandType pType) : Type(pType)  {
    
}

I2CDeviceSessionCommand::I2CDeviceSessionCommand(I2CDeviceSessionCommandType pType, uint8_t pByte) : Type(pType) {
    Data.SingleByte.Byte = pByte;
}

I2CDeviceSessionCommand::I2CDeviceSessionCommand(I2CDeviceSessionCommandType pType, uint8_t* pBytes, uint pLength) : Type(pType) {
    Data.MultipleBytes.Bytes = pBytes;
    Data.MultipleBytes.Len = pLength;

}

I2CDeviceSessionCommand::I2CDeviceSessionCommand(I2CDeviceSessionCommandType pType,uint pDelayInMs, bool pQuiet ) : Type(pType) {
    Data.Execute.DelayInMs = pDelayInMs;
    Data.Execute.Quiet = pQuiet;
}


