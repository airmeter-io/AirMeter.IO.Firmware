#include "Common.h"
#include "CubicCO2Sensor.h"

#include<stdint.h>

class CubicCO2Buffer {
public:
   uint8_t Buffer[MAX_MESSAGE_SIZE];

   CubicCO2Buffer(CubicCO2Command pCommand, int pDataLength) {
      Clear();
      SetPacketType(CubicCO2PacketType::Command);
      SetDataLength(pDataLength);
      SetCommand(pCommand);     
      UpdateChecksum();   
   }

   CubicCO2Buffer() {
      Clear();
   }

   void Clear() {
   memset(Buffer+2, 0, sizeof(Buffer)-2);
   }

   CubicCO2PacketType GetPacketType() {
      return (CubicCO2PacketType)Buffer[0];
   }

   void SetPacketType(CubicCO2PacketType pType) {
      Buffer[0] = (uint8_t)pType;
   }

   int GetPacketLength() {
      auto len = Buffer[1]+3;
      return len > MAX_MESSAGE_SIZE ? MAX_MESSAGE_SIZE : len;
   }

   int GetDataLength() {
      return Buffer[1];
   }

   void SetDataLength(int pLen) {
      Buffer[1] = pLen > MAX_MESSAGE_SIZE ? MAX_MESSAGE_SIZE : pLen;
   }

   CubicCO2Command GetCommand() {
      return (CubicCO2Command)Buffer[2];
   }

   void SetCommand(CubicCO2Command pCommand) {
      Buffer[2] = (uint8_t)pCommand;
   }
   
   uint8_t CalcChecksum() {
      int checksum =0;
      for(int i = 0; i < GetPacketLength()-1; i++)
      {
            checksum += Buffer[i];
      }
      checksum=256-(checksum % 256);
      return checksum;
   }

   void UpdateChecksum() {
      Buffer[GetPacketLength()-1] = (uint8_t)CalcChecksum();
   }

   uint8_t GetChecksum() {
      return Buffer[GetPacketLength()-1];
   }

   void Print(const char* pCaption, int pCount) {
      printf("%s: ", pCaption);
      for(auto i = 0; i < pCount && i < MAX_MESSAGE_SIZE; i++)
         printf("%.2x ", Buffer[i]);
      printf("\n");
   }

   void Print(const char* pCaption) {
      Print(pCaption, GetPacketType());
   }
};

CubicCO2Sensor::CubicCO2Sensor(PinSerial* pSerial) : _serial(pSerial) {
   RegisterValue(&_valDrift);
   RegisterValue(&_valLightAging);
   RegisterValue(&_valCalibrated);
   RegisterValue(&_valHasLessThanRange);
   RegisterValue(&_valHasGreaterThanRange);
   RegisterValue(&_valAbcOpen);
   RegisterValue(&_valAbcBase);
   RegisterValue(&_valAbcCycle);
   RegisterValue(&_valRawValStatus);
   RegisterValue(&_valUnknownStatus);
   RegisterValue(&_valModeStatus);
   
   ReadSoftwareVersion();
   ReadSerialNo();
   ReadAbcInfo();

   ReadSensorInfo();
   ReadSensorStatusText();   
}

CubicCO2ErrorCode CubicCO2Sensor::ExecuteCommand(CubicCO2Buffer& pInput, CubicCO2Buffer& pOutput, int pTries) {
   pInput.UpdateChecksum();
   

   for(auto tries = 0; tries < pTries; tries++) {
      _serial->Write((void*)(pInput.Buffer), pInput.GetPacketLength());
      int len = _serial->Read(pOutput.Buffer, MAX_MESSAGE_SIZE,180);
      if(len != pOutput.GetPacketLength()) {
         pOutput.Print("Invalid Packet Length",len);
         pOutput.Clear();
         vTaskDelay(500 / portTICK_PERIOD_MS);
         continue;
      }

      if(pOutput.GetChecksum()!=pOutput.CalcChecksum()) {
         pOutput.Print("Invalid Checksum",len);
         pOutput.Clear();
         vTaskDelay(500 / portTICK_PERIOD_MS);
         continue;
      }

      switch(pOutput.GetPacketType()) {
         case CubicCO2PacketType::Command :
            return CubicCO2ErrorCode::ReceivedCommand;
         case CubicCO2PacketType::Error :
            if(pOutput.GetDataLength()==1) 
               return (CubicCO2ErrorCode)((int)pOutput.Buffer[3]);
            return CubicCO2ErrorCode::Unknown;
         case CubicCO2PacketType::Response :
            return CubicCO2ErrorCode::Success;
      }      
   }
   return CubicCO2ErrorCode::RetryTimeout;
}

bool CubicCO2Sensor::RefreshValues() {
   CubicCO2Buffer command(CubicCO2Command::ReadValues,1);
   CubicCO2Buffer response;
   command.Print("Refresh Sending");
   auto error = ExecuteCommand(command,response);
   if(error) 
      return false;
      
   response.Print("Refresh Response");
   auto flags = response.Buffer[5];
   _valDrift.value.b = flags & (1<<6);
   _valLightAging.value.b = flags & (1<<5);
   _valCalibrated.value.b = !(flags & (1<<4));
   _valHasLessThanRange.value.b = flags & (1<<3);
   _valHasGreaterThanRange.value.b = flags & (1<<2);
   _hasError = flags & (1<<1);
   _isHeatingUp = !(flags & 1);
  
    _ppm = response.Buffer[3]*256 + response.Buffer[4];  


   return true;
}

void CubicCO2Sensor::ReadSoftwareVersion() {
   CubicCO2Buffer command(CubicCO2Command::GetSoftwareVerison,1);
   CubicCO2Buffer response;
   auto error = ExecuteCommand(command,response);
   if(error) {
      response.Print("Failed read SW ver info");      
      _softVer = "Unknown";
      return;
   } 
   
    _softVer = std::string((char*)response.Buffer+3,10);   
}

void CubicCO2Sensor::ReadSerialNo() {
   CubicCO2Buffer command(CubicCO2Command::GetSerialNo,1);
   CubicCO2Buffer response;
   auto error = ExecuteCommand(command,response);
   if(error) {
      response.Print("Failed read serial no");      
      _serialNo = "Unknown";
      return;
   } 
   response.Print("ReadSerioNo");
    _serialNo = std::to_string((response.Buffer[3] << 8) | response.Buffer[4]);
   _serialNo+="-";
   _serialNo+= std::to_string((response.Buffer[5] << 8) | response.Buffer[6]);
   _serialNo+="-";
   _serialNo+= std::to_string((response.Buffer[7] << 8) | response.Buffer[8]);
   _serialNo+="-";
   _serialNo+= std::to_string((response.Buffer[9] << 8) | response.Buffer[10]);
   _serialNo+="-";
   _serialNo+= std::to_string((response.Buffer[11] << 8) | response.Buffer[12]);
}

void CubicCO2Sensor::ReadAbcInfo() {
   CubicCO2Buffer command(CubicCO2Command::GetABCInfo,1);   
   CubicCO2Buffer response;
   auto error = ExecuteCommand(command,response);
   if(error) {
      response.Print("Failed read ABC info");      
      _valAbcBase.value.i = 0;
      _valAbcCycle.value.i = 0;
      _valAbcOpen.value.b = false;
      return;
   } 

   _valAbcBase.value.i = response.Buffer[7] | (response.Buffer[6] << 8);
   _valAbcCycle.value.i = response.Buffer[5];
   _valAbcOpen.value.b = response.Buffer[4]==0;   
}

int CubicCO2Sensor::GetPPM() {
   return _ppm;
}

int CubicCO2Sensor::GetMaxPPM() {
   return _maxPPM;
}
const std::vector<int> CubicCO2Sensor::availablePPMs = { 2000, 5000 };


const std::vector<int>& CubicCO2Sensor::GetAvailableMaxPPM() const {
   
   return availablePPMs;
}

void CubicCO2Sensor::SetMaxPPM(int pMaxPPM) {
   for(auto avail : GetAvailableMaxPPM())
      if(avail == pMaxPPM && _maxPPM!=pMaxPPM) {
         CubicCO2Buffer command(CubicCO2Command::SetSensorInfo,6);
         command.Buffer[3]=  (pMaxPPM >> 8) & 0xFF;
         command.Buffer[4]=pMaxPPM & 0xFF; 
         command.Buffer[5] = 0x00; // unknown
         command.Buffer[7] = 0x01; // unknown 
         command.Buffer[8] = 0x00; // unknown
         command.UpdateChecksum();
         CubicCO2Buffer response;
         while(ExecuteCommand(command,response, 10)) {
            printf("ERROR: Setting Max PPM retrying\n");
         }         
         response.Print("Set MAX PPM");
         _maxPPM = pMaxPPM;
      }
}


bool CubicCO2Sensor::GetIsHeatingUp() {
   return _isHeatingUp;
}

bool CubicCO2Sensor::GetHasError() {
   return _hasError;
}

void CubicCO2Sensor::SetAbcParameters(bool pOpen, int pCycle, int pBaseCO2Value) {
   CubicCO2Buffer command(CubicCO2Command::SetABCInfo,7);   
   CubicCO2Buffer response;
   
   command.Buffer[3] = 0x64;
   command.Buffer[4] = pOpen ? 0 : 2;
   command.Buffer[5] = pCycle;
   command.Buffer[6] = (pBaseCO2Value >> 8) & 0xFF;
   command.Buffer[7] = (pBaseCO2Value & 0xFF);        
   command.Buffer[8] = 0x64;
   command.UpdateChecksum();
   command.Print("Sending SetABC");
   while(ExecuteCommand(command,response)) {
      response.Print("Failed to set ABC info retrying");  
   }
   _valAbcBase.value.i = pBaseCO2Value;
   _valAbcCycle.value.i = pCycle;
   _valAbcOpen.value.b = pOpen;
   printf("Set ABC Info - Open = %d, Cycle=%d, BaseValue=%d\n", (int)pOpen, (int)pCycle, pBaseCO2Value);
}


void CubicCO2Sensor::StartCalibration(int pBaseCO2Value) {
   CubicCO2Buffer command(CubicCO2Command::StartCalibration,5);   
   CubicCO2Buffer response;
   command.Buffer[3] = (pBaseCO2Value >> 8) & 0xFF;
   command.Buffer[4] = (pBaseCO2Value & 0xFF); 
   command.Buffer[5] =  (2000 >> 8) & 0xFF;;
   command.Buffer[6] =  (2000 & 0xFF);       
   command.UpdateChecksum();
   command.Print("Start Calib");
   printf("Start Calib %d\n", pBaseCO2Value);
   while(ExecuteCommand(command,response)) {
      response.Print("Failed to start calibration, retrying.");     
   }

   response.Print("Started Calibration");
}

void CubicCO2Sensor::ReadSensorStatusText() {
   CubicCO2Buffer command(CubicCO2Command::GetInternalStatusText,1);
   const int MAX_TEXT = 1500;
   char *response = (char *)malloc(MAX_TEXT);
   if(!response) {
      printf("Failed to allocate buffer for response for status text\n");
      return;
   }
   command.UpdateChecksum();
   _serial->Write((void*)(command.Buffer), command.GetPacketLength());
   auto len = _serial->Read(response, MAX_TEXT, 1000);
   if(len>0) {
      response[len] = 0;
      const char split[3] = {0xA1, 0xE6, 0x0};
      char *sensorUnknownStatus = strstr(response,split);
      if(!sensorUnknownStatus) {
         printf("failed to find split\n");
         free(response);
         return;
      }
      *(sensorUnknownStatus) = 0;
      sensorUnknownStatus+=2;
      const char splitMode[3] = {0xA1, 0xEB, 0x0};
      char *sensorModeStatus = strstr(sensorUnknownStatus,splitMode);
      if(!sensorModeStatus) {
         printf("failed to find split mode\n");
         free(response);
         return;
      }
      *sensorModeStatus = 0;
      sensorModeStatus+=2;
      
      _rawValStatus = response;
      _modeStatus = sensorModeStatus;
      _unknownStatus =sensorUnknownStatus;

      free(response);
      return;
   }
   
   
   free(response);   
}

void CubicCO2Sensor::ReadSensorInfo() {
   CubicCO2Buffer command(CubicCO2Command::GetSensorInfo,1);   
   CubicCO2Buffer response;
      
   auto error = ExecuteCommand(command,response);
   if(error) {
      response.Print("Failed to  read sensor info");            
      return;
   } 
   response.Print("ReadSensorInfo");
   _maxPPM = response.Buffer[4] | (response.Buffer[3] << 8);
}



void CubicCO2Sensor::ManualCalibration(int pBaseLinePPM) {
   
   StartCalibration(pBaseLinePPM);
}

void CubicCO2Sensor::EnableABC(int pBaseLinePPM, int pNumberOfDaysPerCycle) {
   _valAbcCycle.value.i = pNumberOfDaysPerCycle;
   _valAbcBase.value.i = pBaseLinePPM;
   _valAbcOpen.value.b = true;
   SetAbcParameters(true, _valAbcCycle.value.i, _valAbcBase.value.i);
}

void CubicCO2Sensor::DisableABC() {
   _valAbcOpen.value.b = false;
   SetAbcParameters(false, _valAbcCycle.value.i, _valAbcBase.value.i);
}



int CubicCO2Sensor::GetBasePPM() {
   return _valAbcBase.value.i;
}

int CubicCO2Sensor::GetDaysPerABCCycle() {
   return _valAbcCycle.value.i;
}

bool CubicCO2Sensor::GetIsABCEnabled() {
   return _valAbcOpen.value.b;
}

void CubicCO2Sensor::SetUnknownMode(uint8_t pMode) {
   CubicCO2Buffer command(CubicCO2Command::SetUnknownMode,2);   
   CubicCO2Buffer response;
   command.Buffer[3] = pMode;
  
   command.UpdateChecksum();
   command.Print("Sending Set Unknwon Mode");
   auto error = ExecuteCommand(command,response);
   if(error) {
      response.Print("Failed to set unknown mode");            
      return;
   } 
}

void CubicCO2Sensor::SetSerialNumber(short pPart1, short pPart2, short pPart3, short pPart4, short pPart5) {
   CubicCO2Buffer command(CubicCO2Command::SetSerialNo,12);   
   CubicCO2Buffer response;
   command.Buffer[4] = (pPart1 >> 8) & 0xFF;
   command.Buffer[5] = (pPart1 & 0xFF);        
   command.Buffer[6] = (pPart2 >> 8) & 0xFF;
   command.Buffer[7] = (pPart2 & 0xFF);      
   command.Buffer[8] = (pPart3 >> 8) & 0xFF;
   command.Buffer[9] = (pPart3 & 0xFF);      
   command.Buffer[10] = (pPart4 >> 8) & 0xFF;
   command.Buffer[11] = (pPart4 & 0xFF);      
   command.Buffer[12] = (pPart5>> 8) & 0xFF;
   command.Buffer[13] = (pPart5 & 0xFF);      
  
   command.UpdateChecksum();
   command.Print("Sending SetSerial");
   auto error = ExecuteCommand(command,response);
   if(error) {
      response.Print("Failed to set serial info");            
      return;
   } 
}

std::string& CubicCO2Sensor::GetSerialNo() {
   return _serialNo;
}

std::string& CubicCO2Sensor::GetDeviceName() {
   return _deviceName;
}
std::string& CubicCO2Sensor::GetSWVersion() {
   return _softVer;
}



