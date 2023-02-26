#include "Common.h"
#include "MhZ19Sensor.h"
#include<stdint.h>
#include<string>

#define MAX_MESSAGE_SIZE  9

class MHZ19Buffer {
public:
   uint8_t Buffer[MAX_MESSAGE_SIZE];

   MHZ19Buffer(MHZ19Command pCommand, int pSensor = 1) {
      Clear();
      SetCommand(pCommand);     
      SetSensor(pSensor);
      UpdateChecksum();
   }

   MHZ19Buffer() {
      Clear();
   }

   void Clear() {
      memset(Buffer, 0, sizeof(Buffer));
   }


   int GetPacketLength() {
     return MAX_MESSAGE_SIZE;
   }

   int GetSensor() {
      return (int)Buffer[1];
   }

   void SetSensor(int pSensor) {
      Buffer[1] = (uint8_t)pSensor;
   }

   MHZ19Command GetCommand() {
      return (MHZ19Command)Buffer[2];
   }

   void SetCommand(MHZ19Command pCommand) {
      Buffer[0] = 0xFF; 
      Buffer[2] = (uint8_t)pCommand;
   }
   
   uint8_t CalcChecksum() {
      char checksum =0;
      for(int i = 1; i < GetPacketLength()-1; i++)
      {
            checksum += Buffer[i];
      }
      checksum=(char)0xff-checksum;
      checksum++;
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
      Print(pCaption, GetPacketLength());
   }
};

const std::string GROUP_MHZ19x = "MHZ19x";

const ValueIdentifier MHZ19x_TEMPERATURE         = { .Id = 1020, .Grouping = GROUP_MHZ19x, .Name = "Temperature" };
const ValueIdentifier MHZ19x_HUMIDITY            = { .Id = 1021, .Grouping = GROUP_MHZ19x, .Name = "ABCCount"} ;
const ValueIdentifier MHZ19x_MEASUREMENTINTERVAL = { .Id = 1022, .Grouping = GROUP_MHZ19x, .Name = "ABCTick"};
const ValueIdentifier MHZ19x_ALTITUDE            = { .Id = 1023, .Grouping = GROUP_MHZ19x, .Name = "TempAdjustment"};

MhZ19Sensor::MhZ19Sensor(PinSerial* pSerial) : _serial(pSerial){
   _deviceName = "Winsen MH-Zxx Family CO2 Sensor";

   AddValueSource(new ValueSource(*this, MHZ19x_TEMPERATURE, Int, Dimensionless, _valAbcCount, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this, MHZ19x_HUMIDITY, Int, Dimensionless, _valAbcTick, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this, MHZ19x_MEASUREMENTINTERVAL, Int, Dimensionless, _valTemp, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this, MHZ19x_ALTITUDE, Int, Dimensionless, _valTempAdjustment, GET_LATEST_DATA));
   
   _valBasePPM.i = 400;
   _valMaxBasePPM.i = 400;
   _valMinBasePPM.i = 400;
   _valHoursPerAbcCycle.i = 24;
   _valMaxHoursPerAbcCycle.i = 24;
   _valMinHoursPerAbcCycle.i = 24;
   
   char buf[300];
   while(true) {
    int read = _serial->Read(buf, sizeof(buf), 250);
    printf("Read %d bytes from MH Z19: ", read);
    for(auto i = 0; i < read; i++)
      printf("%.2x ", buf[i]);
    printf("\n");
    if(read==0) break;
   }
   ReadSoftwareVersion();
   ReadDetectionRange();   
   ReadTemperatureAdjustment();
   ReadABCStatus();
   ReadIDString();
}



const std::string& MhZ19Sensor::GetValuesSourceName() const {   
   return SOURCE_NAME;
}


bool MhZ19Sensor::ExecuteCommand(MHZ19Buffer& pCommand, MHZ19Buffer& pResponse) {
   _serial->Write((void*)pCommand.Buffer, pCommand.GetPacketLength());
   int len = _serial->Read(pResponse.Buffer, sizeof(pResponse.Buffer), 350);
   if(len!=sizeof(pResponse.Buffer)) {
      return false;
   }
   if(pResponse.CalcChecksum()!=pResponse.GetChecksum()) {  
      return false;
   }
    return true;
}


bool MhZ19Sensor::RefreshValues() {
   MHZ19Buffer command(MHZ19Command::ReadCO2);
   MHZ19Buffer response;
   if(!ExecuteCommand(command,response)) {
      response.Print("Failed to read CO2");
      return false;
   }
   response.Print("Read CO2");
   _valCo2.i = response.Buffer[2]*256 + response.Buffer[3];
   _valCo2Unfiltered.i = response.Buffer[2]*256 + response.Buffer[3];
   _valCo2Uncompensated.i = response.Buffer[2]*256 + response.Buffer[3];
   _valCo2UnfilteredUncompensated.i = response.Buffer[2]*256 + response.Buffer[3];

   _valTemp.i = response.Buffer[4] - _valTempAdjustment.i;
   _valAbcTick.i = response.Buffer[5]*256 + response.Buffer[6];
   _valAbcCount.i = response.Buffer[7];
   return true;
}




char MhZ19Sensor::CalcChecksum(char *pPacket) {
    char checksum =0;
    for(int i = 1; i < 8; i++)
    {
         checksum += pPacket[i];
    }
    checksum=(char)0xff-checksum;
    checksum++;
    return checksum;
}


void MhZ19Sensor::ManualCalibration(int pBaseLinePPM) {
   MHZ19Buffer command(MHZ19Command::ZeroCalibration);
   MHZ19Buffer response;
   while(!ExecuteCommand(command,response)) {
      response.Print("Failed to start manual calibration");
   }
   response.Print("Started manual Calibration");
}

void MhZ19Sensor::EnableABC( int pBaseLinePPM, int pNumberOfHoursPerCycle) {
   MHZ19Buffer command(MHZ19Command::SetAutoCalibration);
   MHZ19Buffer response;
   command.Buffer[3] = 0xA0;
   command.UpdateChecksum();
   while(!ExecuteCommand(command,response)) {
      response.Print("Failed to enable ABC");     
   }
   response.Print("Enabled ABC");
   ReadABCStatus();
}

void MhZ19Sensor::DisableABC() {
   MHZ19Buffer command(MHZ19Command::SetAutoCalibration);
   MHZ19Buffer response;
   while(!ExecuteCommand(command,response)) {
      response.Print("Failed to disable ABC");
   }
   response.Print("Disabled ABC");
   ReadABCStatus();  
}



void MhZ19Sensor::ReadSoftwareVersion() {
   MHZ19Buffer command(MHZ19Command::GetFirmwareVersion);
   MHZ19Buffer response;
   if(!ExecuteCommand(command,response)) {
      response.Print("Failed to read software version");
      return;
   }

   response.Print("Successfully read");    
   _softwareVersion = std::string((char*)response.Buffer+2,response.GetPacketLength()-3);   
}

void MhZ19Sensor::ReadIDString() {
   _serialNo = "";
   MHZ19Buffer command(MHZ19Command::ReadIDString);
   auto cont = true;
   for(int i = 1; i <= 255 && cont;i++) {

      command.Buffer[3]  = (uint8_t)i;
      command.UpdateChecksum();
      MHZ19Buffer response;
      
      while(!ExecuteCommand(command,response)) {  }
      for(auto j = 4; j <=6; j++)
         if(response.Buffer[j] == 0xFF) 
            {
               cont = false;
               break;
            }
         else 
            _serialNo+=(char)response.Buffer[j];       
   }
}


void MhZ19Sensor::ReadDetectionRange() {
   MHZ19Buffer command(MHZ19Command::GetDetectionRange);
   MHZ19Buffer response;

   while(!ExecuteCommand(command,response)) {
      response.Print("Failed to read detection range");
      vTaskDelay(500 / portTICK_PERIOD_MS);
   }
   response.Print("Read Detection Range");
   _valMaxPPM.i = response.Buffer[4]*256 + response.Buffer[5];
}

void MhZ19Sensor::ReadTemperatureAdjustment() {
   MHZ19Buffer command(MHZ19Command::GetTemplCalibration);
   MHZ19Buffer response;
  // command.Buffer[3] = 1;
   command.UpdateChecksum();
   while(!ExecuteCommand(command,response)) {
      response.Print("Failed to read temperature adjustment value");
      //return;
   }
   response.Print("Read temperature adjustment value.");
   _valTempAdjustment.i = response.Buffer[3];
}

void MhZ19Sensor::ReadABCStatus() {
   MHZ19Buffer command(MHZ19Command::GetAutoCalibration);
   MHZ19Buffer response;
   if(!ExecuteCommand(command,response)) {
      response.Print("Failed to read ABC Status");
      return;
   }
   response.Print("Read ABC Status");
   _valIsAbcEnabled.b = response.Buffer[7] == 1;
}


