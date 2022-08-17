#include "ViewCommands.h"
#include "ValueModel.h"
#include "DataManagerQuery.h"
#include "Json.h"

#include "Utility.h"
#include <string>
#include <vector>

GetLatestDataCommand::GetLatestDataCommand(ValueModel& pValues) : _values(pValues) {
    
}

void GetLatestDataCommand::Process(Json& pJson,Json& pResult)  {
    pResult.CreateStringProperty("Status", "true");
    pResult.CreateBoolProperty("HasCo2", _values.CO2!=nullptr);
    if(_values.CO2) {
        pResult.CreateNumberProperty("Co2", _values.CO2->GetPPM());    
        pResult.CreateNumberProperty("Co2Max", _values.CO2->GetMaxPPM());    
        pResult.CreateBoolProperty("Co2Heating", _values.CO2->GetIsHeatingUp());
        pResult.CreateBoolProperty("Co2Error", _values.CO2->GetHasError());
        pResult.CreateNumberProperty("Co2AdditionalCount", (int)(_values.CO2->GetValues().size()));
        pResult.CreateStringProperty("Co2Name", _values.CO2->GetDeviceName().c_str()); 
        pResult.CreateStringProperty("Co2SwVersion", _values.CO2->GetSWVersion().c_str()); 
        pResult.CreateStringProperty("Co2SerialNo", _values.CO2->GetSerialNo().c_str()); 
        std::vector<Json*> additonalValues;
        for(auto additional : _values.CO2->GetValues()) {
            auto additionalValJson = new Json();
            additonalValues.push_back(additionalValJson);
            additionalValJson->CreateStringProperty("Name", additional->name);
            switch(additional->type){
            case Bool : 
                additionalValJson->CreateBoolProperty("Value", additional->value.b);
                break;
            case Int :
                additionalValJson->CreateNumberProperty("Value", additional->value.i);
                break;
            case Double:  
                additionalValJson->CreateNumberProperty("Value", additional->value.d);
                break;
            case String:
                additionalValJson->CreateStringProperty("Value", *additional->value.s);
                break;
            }
        }
        pResult.CreateArrayProperty("Co2AdditionalValues", additonalValues);
    }
    
    pResult.CreateStringProperty("Temp", _values.Bme280.GetTemperatureStr());
    pResult.CreateStringProperty("Pressure", _values.Bme280.GetPressureStr(2));
    pResult.CreateStringProperty("Humidity", _values.Bme280.GetHumidityStr()); 
    pResult.CreateStringProperty("Time", GetCurrentIsoTimeString()); 
    
}

std::string GetLatestDataCommand::GetName() {
    return "GETLATEST";
}

GetHistoricalDataCommand::GetHistoricalDataCommand(DataManager& pManager) : _manager(pManager) {
    
}

void GetHistoricalDataCommand::Process(Json& pJson,Json& pResult)  {
    if(pJson.HasProperty("Mode")) {
        auto mode = pResult.GetStringProperty("Mode");
        if(mode == "Counts") {

            pResult.CreateStringProperty("Status", "true");
            return;
        }  
    } 
    pResult.CreateStringProperty("Status", "false");    
}

void GetHistoricalDataCommand::ProcessFullResponse(Json& pJson, HttpRequest& pReq ) {
    if(pJson.HasProperty("Mode")) {
        auto mode = pJson.GetStringProperty("Mode");
        if (mode == "Read" && pJson.HasProperty("From") && pJson.HasProperty("To")) {
            auto from = pJson.GetUIntProperty("From");
            auto to = pJson.GetUIntProperty("To");
            auto query = _manager.StartQuery(from,to);
            const uint NumRows = 128;
            auto rows = (DataEntry*) calloc(NumRows, sizeof(DataEntry));
            auto read = query->ReadEntries(rows, NumRows);

            pReq.SetType("application/json;charset=UTF-8");

            const uint BufSize = 4096;
            auto buf = (char*)malloc(BufSize);
            snprintf(buf, BufSize, "{ \"Status\": \"true\", \"Readings\": [");
            auto bufIndex = strlen(buf);
            auto first = true;
            while(read>0) {
                for(auto i = 0; i < read; i++) {
                    if(first) 
                        snprintf(
                            (char*)buf+bufIndex, 
                            99, 
                            "{\"TS\": \"%u\",\"C\":\"%u\",\"T\":\"%d.%u\",\"H\":\"%u.%u\",\"P\":\"%u.%u\"}",
                            (uint)rows[i].TimeStamp,
                            rows[i].CO2,
                            rows[i].Temp/100,
                            abs(rows[i].Temp%100),   
                            rows[i].Humidity/100,
                            rows[i].Humidity%100,
                            800+rows[i].Pressure/100,
                            rows[i].Pressure%100);
                    else
                        snprintf(
                            buf+bufIndex, 
                            99, 
                            ",{\"TS\": \"%u\",\"C\":\"%u\",\"T\":\"%d.%u\",\"H\":\"%u.%u\",\"P\":\"%u.%u\"}",
                            (uint)rows[i].TimeStamp,
                            rows[i].CO2,
                            rows[i].Temp/100,
                            abs(rows[i].Temp%100),   
                            rows[i].Humidity/100,
                            rows[i].Humidity%100,
                            800+rows[i].Pressure/100,
                            rows[i].Pressure%100);
                    first = false;
                    bufIndex+=strlen((char*)buf + bufIndex);
                    if(bufIndex+100>=BufSize) {
                         pReq.SendChunk(buf, bufIndex);
                         bufIndex = 0;
                    } 
                   
                }
              
                read = query->ReadEntries(rows, NumRows);
  

            }

            const char *leadOut = "]}";
            auto len = strlen(leadOut);
            if(len+bufIndex>=BufSize) {
                pReq.SendChunk(buf, bufIndex);
                bufIndex = 0;
            } 
            memcpy(buf+bufIndex, leadOut, len);
            bufIndex+=len;
            pReq.SendChunk(buf, bufIndex);

            free(buf);
            pReq.FinishedChunks();
            delete query;
            free(rows);
            return;
        } 
    } 
    Json result;
    Process(pJson,result);
    pReq.SendResponse("application/json;charset=UTF-8",result.Print());
}

std::string GetHistoricalDataCommand::GetName() {
    return "GETDATA";
}