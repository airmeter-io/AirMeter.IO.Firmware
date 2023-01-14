#include "DevicePersonality.h"
#include "Json.h"
#include <string>
#include <stdio.h>

DevicePersonality::DevicePersonality(
        const std::string pProfile,
        const std::string pBuildVersion,
        const std::string pBuildAsset,
        const UartConfig& pUartConfig,
        const DisplayConfig& pDisplayConfig) : 
        _profile(pProfile),
        _buildVersion(pBuildVersion),
        _buildAsset(pBuildAsset),
        _uartConfig(pUartConfig),
        _displayConfig(pDisplayConfig) {
    
}

DevicePersonality* DevicePersonality::Load() {    
    auto f = fopen("/dev/device.json", "rb");

    std::string profile = "default";
    std::string buildVersion = "MASTER";
    std::string buildAsset = "MASTERBUILD";
    UartConfig uartConfig = { .Enabled = false,.UartNum = CONFIG_CO2_UART_NUM, .OverrideGpio = false, .RxGpio=GPIO_NUM_MAX, .TxGpio=GPIO_NUM_MAX};
    DisplayConfig displayConfig = { .Enabled = false, .Driver = "", .DriverConfig = nullptr };


    if (f != NULL) {        
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET); 
        auto* rawJson = (char *)calloc(fsize + 1,1);
        fread(rawJson, 1, fsize, f);
        fclose(f);

        Json json(rawJson);
        
        if(json.HasProperty("profile"))
            profile = json.GetStringProperty("profile");
        if(json.HasProperty("version"))
            buildVersion = json.GetStringProperty("buildVersion");        
        if(json.HasProperty("asset"))
            buildAsset = json.GetBoolProperty("buildAsset");
        
        if(json.HasObjectProperty("display")) {
            auto displayProp = json.GetObjectProperty("display");
            if(displayProp->HasProperty("enabled")) 
                displayConfig.Enabled = displayProp->GetBoolProperty("enabled");
            if(displayProp->HasProperty("driver")) 
                displayConfig.Driver = displayProp->GetStringProperty("driver");
            
            if(displayProp->HasObjectProperty("config")) {
                auto configProp = displayProp->GetObjectProperty("config");
                displayConfig.DriverConfig = new Json(configProp->Print());
                delete configProp;
            }
            delete displayProp;
        }

        if(json.HasObjectProperty("uart")) {
            auto uartProp = json.GetObjectProperty("uart");
            if(uartProp->HasProperty("enabled")) 
                uartConfig.Enabled = uartProp->GetBoolProperty("enabled");
            if(uartProp->HasProperty("uartNum"))
                uartConfig.UartNum = uartProp->GetIntProperty("uartNum");
            if(uartProp->HasObjectProperty("gpio")) {
                auto gpioProp = uartProp->GetObjectProperty("gpio");
                if(gpioProp->HasProperty("rx") && gpioProp->HasProperty("tx")) {
                    uartConfig.OverrideGpio = true;
                    auto rxGpio =  gpioProp->GetIntProperty("rx");
                    auto txGpio =  gpioProp->GetIntProperty("tx");
                    if(rxGpio && txGpio && rxGpio < (int)GPIO_NUM_MAX && txGpio < (int)GPIO_NUM_MAX) {
                        uartConfig.RxGpio = (gpio_num_t)rxGpio;
                        uartConfig.TxGpio = (gpio_num_t)txGpio;
                    }
                }
                delete gpioProp;
            } 
            delete uartProp;           
        }
        
        
        free(rawJson);
    }

    printf("Profile: %s\n",profile.c_str());
    printf("Build Version: %s\n",buildVersion.c_str());
    printf("Build Asset: %s\n",buildAsset.c_str());
    printf("Uart Enabled: %s\n",uartConfig.Enabled? "true" : "false");
    printf("Override GPIO: %s\n",uartConfig.OverrideGpio? "true" : "false");
    printf("TX/RX: %d/%d\n",(int)uartConfig.TxGpio, (int)uartConfig.RxGpio);
    return new DevicePersonality(
            profile,
            buildVersion,
            buildAsset,
            uartConfig,
            displayConfig);

}

const std::string& DevicePersonality::GetProfile() const {
    return _profile;
}

const std::string& DevicePersonality::GetBuildVersion() const {
    return _buildVersion;
}

const std::string& DevicePersonality::GetBuildAsset() const {
    return _buildAsset;
}

const UartConfig& DevicePersonality::GetUartConfig() const {
    return _uartConfig;
}

const DisplayConfig& DevicePersonality::GetDisplayConfig() const {
    return _displayConfig;
}

