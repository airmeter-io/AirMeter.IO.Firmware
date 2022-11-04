#pragma once
#pragma once

#include "Common.h"
#include <string>
#include <vector>
#include "Json.h"

extern "C" {
    #include "driver/uart.h"
}

typedef struct {
    bool Enabled;
    uart_port_t UartNum;
    bool OverrideGpio;
    gpio_num_t RxGpio;
    gpio_num_t TxGpio;
} UartConfig;

typedef struct {
    bool Enabled;
    std::string Driver;
    Json* DriverConfig;
} DisplayConfig;

class DevicePersonality {
   const std::string _profile;
   const std::string _buildVersion;
   const std::string _buildAsset;
   const UartConfig _uartConfig;
   const DisplayConfig _displayConfig;
   
   
   DevicePersonality(
        const std::string pProfile,
        const std::string pBuildVersion,
        const std::string pBuildAsset,
        const UartConfig& pUartConfig,
        const DisplayConfig& pDisplayConfig);
public:
    static DevicePersonality* Load();

    
    const std::string& GetProfile() const;
    const std::string& GetBuildVersion() const;
    const std::string& GetBuildAsset() const;

    const UartConfig& GetUartConfig() const;
    const DisplayConfig& GetDisplayConfig() const;
};