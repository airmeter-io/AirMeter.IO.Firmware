#include "DriverSSD16xx.h"


#include "DrawControlSSD16xx.h"
#include "Utility.h"

DrawControl* DriverSSD16xx::LoadDriver(Json* pConfiguration ) {    
    SSD16xxConfiguration config;
    if(pConfiguration->HasProperty("panel"))
    {
        auto panel = pConfiguration->GetStringProperty("panel");

        if(LoadPanel(panel, config)) {
            _io = new EpdSpi();
            _io->init(4,false);
            _ssd1680 = new SSD1680(*_io);        
            return new DrawControlSSD16xx(*_ssd1680, config);
        } else {
            printf("Failed to load panel %s\n", panel.c_str());
        }
    } else 
        printf("Panel Property not found\n");

    printf("Failed to initialise SSD1680\n");
    return new NullDrawControl();   
}

bool DriverSSD16xx::LoadPanel(std::string pName, SSD16xxConfiguration& pInfo) {
    auto f = fopen("/dev/ssd16xx.json", "rb");
    auto result = false;
    if (f != NULL) {        
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET); 

        auto* rawJson = (char *)calloc(fsize + 1,1);
        fread(rawJson, 1, fsize, f);
        fclose(f);

        Json json(rawJson);
        if(json.HasObjectProperty(pName)) {
            result = LoadPanelInfo(json.GetObjectProperty(pName), pInfo);
        }
        free(rawJson);
    }
    return result;
}

bool DriverSSD16xx::LoadPanelInfo(Json* pJson, SSD16xxConfiguration& pInfo) {
    if(!pJson->HasProperty("Width") || !pJson->HasProperty("Height")) return false;
    pInfo.Width = pJson->GetUIntProperty("Width");
    pInfo.Height = pJson->GetUIntProperty("Height");
    if(pJson->HasProperty("RamXOffset")) pInfo.RamXOffset = pJson->GetUIntProperty("RamXOffset");
    if(pJson->HasProperty("PingPong")) pInfo.EnablePingPong = pJson->GetUIntProperty("PingPong");
    memset(&pInfo.LUTPartialUpdate, 0, sizeof(pInfo.LUTPartialUpdate));
    if(pJson->HasObjectProperty("PartialLut")) {
        auto partial = pJson->GetObjectProperty("PartialLut");
        pInfo.LUTPartialUpdate.Enable = true;
        if(!partial->HasProperty("GateDrivingVoltage") ||
           !partial->HasProperty("VCOM") ||
           !partial->HasProperty("Data") ||
           !partial->HasObjectProperty("SourceDrivingVoltage"))
           return false;
        
        pInfo.LUTPartialUpdate.GateDrivingVoltage = partial->GetIntProperty("GateDrivingVoltage");
        pInfo.LUTPartialUpdate.VCOM = partial->GetIntProperty("VCOM");
        auto source = partial->GetObjectProperty("SourceDrivingVoltage");
        if(!source->HasProperty("Vsh1") || !source->HasProperty("Vsh2") || !source->HasProperty("Vsl"))
            return false;
        pInfo.LUTPartialUpdate.SourceDrivingVoltage.Vsh1 = source->GetIntProperty("Vsh1");
        pInfo.LUTPartialUpdate.SourceDrivingVoltage.Vsh2 = source->GetIntProperty("Vsh2");
        pInfo.LUTPartialUpdate.SourceDrivingVoltage.Vsl = source->GetIntProperty("Vsl");

        if(!HexStringToBytes(partial->GetStringProperty("Data"), &pInfo.LUTPartialUpdate.Data, &pInfo.LUTPartialUpdate.DataLength)) {
            printf("Failed to parse LUT hex string\n");
        }
    }

    return true;        
}

std::string DriverSSD16xx::GetName() const {
    return "ssd16xx";
}
