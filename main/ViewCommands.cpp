#include "ViewCommands.h"
#include "DataManagerQuery.h"
#include "Json.h"
#include "ValueController.h"
#include "Utility.h"
#include <string>
#include <vector>

GetLatestDataCommand::GetLatestDataCommand()  {
    
}

void GetLatestDataCommand::Process(Json& pJson,Json& pResult)  {
    pResult.CreateStringProperty("Status", "true");

    for(auto groupPair : ValueController::GetCurrent().GetGroups()) {
        Json* groupJson = nullptr;
        for(auto keyPair : groupPair.second->SourcesByName) {
            auto source = keyPair.second->DefaultSource;
            if(source->GetFlags() & GET_LATEST_DATA ) {
                if(groupJson == nullptr) 
                    groupJson = pResult.CreateObjectProperty(groupPair.first);
                source->SerialiseToJsonProperty(*groupJson);
            }            
        }   
        if(groupJson!=nullptr)
            delete groupJson; 
    }      
    
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