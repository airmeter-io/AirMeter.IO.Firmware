#pragma once


enum DataPointType {
    Full = 0,
    Compact = 1,
    VeryCompact = 2
};

typedef struct  {
    uint Type:2; // DataPointType::Full
    int Temp:15;
    uint16_t CO2 : 15; 
    uint16_t Humidity;  
    uint16_t Pressure;
    uint32_t TimeStamp;
} __attribute__((packed)) DataPointFull;

typedef struct {
    uint Type:2; // DataPointType::Compact
    uint TimeDelta:14;
    int8_t CO2Delta; 
    int8_t TempDelta; 
    int8_t HumidityDelta;  
    int8_t PressureDelta;
} __attribute__((packed)) DataPointCompact;


typedef struct {
    uint Type:2; // DataPointType::VeryCompact
    uint TimeDelta:9;
    int CO2Delta : 5; 
    int TempDelta : 5; 
    int HumidityDelta : 6;  
    int PressureDelta : 5;
} __attribute__((packed)) DataPointVeryCompact;

typedef struct {
    uint32_t Magic;
    time_t BlockStartTime;
    time_t BlockEndTime;
    time_t WriteTime;
    uint DataLength;
} __attribute__((packed)) FlashDataBlockHeader;