#include "SenseairModBusSensor.h"

enum SenseairModBusFunction {
    ReadHoldingRegisters = 0x3,
    ReadInputRegisters = 0x4,
    WriteMultipleRegisters = 0x10,
    ReadDeviceIdentification = 0x0E,
    ReadDeviceIdentification2 = 0x2B   
};

class SenseairModBusBuffer {
    uint8_t *_buffer;
    uint _length;

    uint16_t CalcCRC()
    {
        uint16_t crc = 0xFFFF;
        for (int pos = 0; pos < _length-2; pos++) {
            crc ^= (uint16_t)_buffer[pos];
            for (int i = 8; i != 0; i--) { 
                if ((crc & 1) != 0) { 
                    crc >>= 1;   
                    crc ^= 0xA001;
                }
                else                     
                    crc >>= 1;          
                }
        }
        return crc;  
    }
public:
    SenseairModBusBuffer( uint pPayloadLength) {
        _buffer = (uint8_t*)calloc(pPayloadLength+4,1);
        _length = pPayloadLength+4;
    }

    SenseairModBusBuffer(uint8_t pDeviceAddress,SenseairModBusFunction pFunction, uint pPayloadLength) : SenseairModBusBuffer(pPayloadLength) {
        _buffer[0] = pDeviceAddress;
        _buffer[1] = (uint8_t)pFunction;
    }

    bool CheckCRC() {
        auto crc = CalcCRC();
        return _buffer[_length-1]==crc/256 && _buffer[_length-2]==crc%256;
    }

    void UpdateCRC() {
        auto crc = CalcCRC();
        _buffer[_length-1]=crc/256;
        _buffer[_length-2]=crc%256;
    }

    uint8_t* GetBuffer() {
        return _buffer;
    }

    int GetLength() {
        return _length;
    }

    SenseairModBusFunction GetFunction() {
        return (SenseairModBusFunction)_buffer[1];
    }

    void Print(const char *pPrefix) {
        printf("%s: ",pPrefix);
        for(auto i = 0; i <_length ; i++)
            printf("%.2x ",_buffer[i]);            
        printf("\n");
    }



    ~SenseairModBusBuffer() {
        free(_buffer);
    }
};

class SenseairModBusCommand {
public: 
    virtual SenseairModBusBuffer& GetCmd() = 0;
    virtual SenseairModBusBuffer& GetResponse() = 0;
};

class ReadRegistersFunction : public SenseairModBusCommand {
    SenseairModBusBuffer* _cmd;
    SenseairModBusBuffer* _response;
public:
    ReadRegistersFunction( uint8_t pDeviceAddress,SenseairModBusFunction pFunction, uint8_t pStartRegister, uint8_t pCount) {
        _cmd = new SenseairModBusBuffer(pDeviceAddress,pFunction,4);
        _cmd->GetBuffer()[_cmd->GetLength()-6]=pStartRegister/256;
        _cmd->GetBuffer()[_cmd->GetLength()-5]=pStartRegister%256;        
        _cmd->GetBuffer()[_cmd->GetLength()-4]=pCount/256;
        _cmd->GetBuffer()[_cmd->GetLength()-3]=pCount%256;
        _response = new SenseairModBusBuffer(1+pCount*2);
    }

    ~ReadRegistersFunction() {
        delete _cmd;
        delete _response;
    }
     SenseairModBusBuffer& GetCmd() override{
        return *_cmd;
    }
    
    SenseairModBusBuffer& GetResponse() override {
        return *_response;
    }

    

    bool IsResponseValid() {
        return _response->CheckCRC();
    }
};


class WriteRegistersFunction : public SenseairModBusCommand {
    SenseairModBusBuffer* _cmd;
    SenseairModBusBuffer* _response;
public:
    WriteRegistersFunction( uint8_t pDeviceAddress, uint8_t pStartRegister, uint8_t pCount, uint8_t *pRegisters) {
        _cmd = new SenseairModBusBuffer(pDeviceAddress,SenseairModBusFunction::WriteMultipleRegisters,5+(pCount*2));
        _cmd->GetBuffer()[2]=pStartRegister/256;
        _cmd->GetBuffer()[3]=pStartRegister%256;        
        _cmd->GetBuffer()[4]=0;
        _cmd->GetBuffer()[5]=pCount;
        _cmd->GetBuffer()[6]=pCount*2;
        _cmd->Print("Before memcpy");
        memcpy(_cmd->GetBuffer()+7, pRegisters, pCount*2);
        _cmd->Print("After memcpy");
        _response = new SenseairModBusBuffer(4);
    }

    ~WriteRegistersFunction() {
        delete _cmd;
        delete _response;
    }

     SenseairModBusBuffer& GetCmd() override{
        return *_cmd;
    }
    
    SenseairModBusBuffer& GetResponse() override {
        return *_response;
    }
};

SenseairModBusSensor::SenseairModBusSensor(PinSerial* pSerial) : _serial(pSerial) {
    _deviceName = "Senseair Modbus";
    ReadInputRegistersInfo();
    ReadHoldingRegistersInfo();
}



const std::string& SenseairModBusSensor::GetValuesSourceName() const {
   return SOURCE_NAME;
}


SenseairModBusSensor::~SenseairModBusSensor() {
}

bool SenseairModBusSensor::SendCommand(SenseairModBusCommand& pCommand) {
     for(auto i = 0; i < 10 || !_firstCommand; i++) {
        pCommand.GetCmd().UpdateCRC();
        pCommand.GetCmd().Print("Sending");
        _serial->Write(pCommand.GetCmd().GetBuffer(),pCommand.GetCmd().GetLength());
        esp_task_wdt_reset();
        auto read = _serial->Read(
            pCommand.GetResponse().GetBuffer(), 
            pCommand.GetResponse().GetLength(),
            200+pCommand.GetResponse().GetLength());
        esp_task_wdt_reset();
        pCommand.GetResponse().Print("Got");
        if(read==pCommand.GetResponse().GetLength() && pCommand.GetResponse().CheckCRC()) {
            _firstCommand = false;
            return true;
        }
        printf("Error read %d but expected %d bytes\n",read ,pCommand.GetResponse().GetLength());
        //pCommand.GetResponse().Print("");
     }
     return false;
    
 }

#define DEVICE_ADDRESS 0x68
bool SenseairModBusSensor::RefreshValues() {
    ReadRegistersFunction cmd(DEVICE_ADDRESS, SenseairModBusFunction::ReadInputRegisters, 0, 11);
    if(SendCommand(cmd)){
        auto buffer = cmd.GetResponse().GetBuffer();
        UpdateErrorStatus(buffer+3);
        _valCo2.i = buffer[9]*256+buffer[10];
        _valCo2Unfiltered.i = buffer[9]*256+buffer[10];
        _valCo2Uncompensated.i = buffer[9]*256+buffer[10];
        _valCo2UnfilteredUncompensated.i = buffer[9]*256+buffer[10];

        ushort tempRaw = buffer[11]*256 + buffer[12];
        ushort measurementCount = buffer[16];
        ushort measurementSeconds = buffer[17]*256 + buffer[18];
        measurementSeconds*=2;
        _temp = std::to_string(tempRaw/100)+"."+std::to_string(tempRaw % 100);

        _measurementInfo = std::to_string(measurementSeconds) + "s since reading #"+std::to_string(measurementCount);
    }     
    return true;
}

void SenseairModBusSensor::ReadInputRegistersInfo() {
    ReadRegistersFunction cmd(DEVICE_ADDRESS, SenseairModBusFunction::ReadInputRegisters, 23, 8);
   
    if(SendCommand(cmd)) {
        auto buffer = cmd.GetResponse().GetBuffer();
         _valFirmwareType.i = buffer[3]*256 + buffer[4];
         _softwareVersion = std::to_string(buffer[13])+"."+std::to_string(buffer[14]);
         char buf[10];
        snprintf(buf, sizeof(buf), "%.2X%.2X%.2X%.2X", buffer[15],buffer[16], buffer[17],buffer[18]);
        _serialNo = buf;
    }
}

void SenseairModBusSensor::ReadHoldingRegistersInfo() {
    ReadRegistersFunction cmd(DEVICE_ADDRESS, SenseairModBusFunction::ReadHoldingRegisters, 0, 20);

    SendCommand(cmd);
    auto values = cmd.GetResponse().GetBuffer()+3;
    auto measurementMode =  values[21];
    _valSingleMeasurementMode.b = measurementMode == 1;
    _valMeasurementPeriod.i = values[22]*256 + values[23];
    _valMeasurementNoSamples.i = values[24]*256 + values[25];
    _valAbcPeriod.i = values[26]*256 + values[27];
    _valAbcTarget.i = values[30]*256 + values[31];
    _valFilter.i = values[33];
    auto meterControl = values[37];
    _meterControl = "";

    if((meterControl & (int)MeterControlModes::nRDYEnabled)==0) _meterControl+="nRDY ";
    if((meterControl & (int)MeterControlModes::ABCEnabled)==0) _meterControl+="ABC ";
    if((meterControl & (int)MeterControlModes::StaticIREnabled)==0) _meterControl+="StaticIR ";
    if((meterControl & (int)MeterControlModes::DynmaicIREnabled)==0) _meterControl+="DynmaicIR ";
    if((meterControl & (int)MeterControlModes::PressureCompensationEnabled)==0) _meterControl+="PressureCompensation ";
    if((meterControl & (int)MeterControlModes::nRDNYPinInvert)==0) _meterControl+="nRDNYPinInvert ";

    if(measurementMode!=0 && measurementMode!=0) {
        uint8_t registers[2];
        registers[0] = 0;
        registers[1] = 0;
        WriteRegistersFunction writeCmd(DEVICE_ADDRESS, 0xa, 1, (uint8_t*)registers);
        if(SendCommand(writeCmd)) {
            printf("Updated Single Measurement Mode\n");
        } else {
            printf("Failed Update Single Measurement Mode\n");
        }
    }
    if(_valMeasurementNoSamples.i > 50) {
        uint8_t registers[4];
        registers[0] = 0;
        registers[1] = 16;
        registers[2] = 0;
        registers[3] = 8;
        WriteRegistersFunction writeCmd(DEVICE_ADDRESS, 0xb, 2, (uint8_t*)registers);
        if(SendCommand(writeCmd)) {
            printf("Updated Measurement Periods\n");
        } else {
            printf("Failed to update measurement periods\n");
        }
    }
}

void SenseairModBusSensor::DisableABC() {

}

void SenseairModBusSensor::ManualCalibration(int pBaseLinePPM) {

}

void SenseairModBusSensor::EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) {

}