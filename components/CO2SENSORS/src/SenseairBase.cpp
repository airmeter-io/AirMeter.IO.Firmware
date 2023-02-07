#include "SenseairBase.h"

const std::string GROUP_SUNRISE = "Sunrise";

const ValueIdentifier SUNRISE_MEASUREMENTINFO       = { .Id = 1060, .Grouping = GROUP_SUNRISE, .Name = "MeasurementInfo" };
const ValueIdentifier SUNRISE_FIRMWARETYPE          = { .Id = 1061, .Grouping = GROUP_SUNRISE, .Name = "FirmwareType"} ;
const ValueIdentifier SUNRISE_FIRMWARETEMPERATURE   = { .Id = 1062, .Grouping = GROUP_SUNRISE, .Name = "FirmwareTemperature"};
const ValueIdentifier SUNRISE_ERROR                 = { .Id = 1063, .Grouping = GROUP_SUNRISE, .Name = "Error"};
const ValueIdentifier SUNRISE_SINGLEMEASUREMENTMODE = { .Id = 1064, .Grouping = GROUP_SUNRISE, .Name = "SingleMeasurementMode"};
const ValueIdentifier SUNRISE_MEASUREMENTPERIOD     = { .Id = 1065, .Grouping = GROUP_SUNRISE, .Name = "MeasurementPeriod"};
const ValueIdentifier SUNRISE_NUMSAMPLES            = { .Id = 1066, .Grouping = GROUP_SUNRISE, .Name = "NumSamples"};
const ValueIdentifier SUNRISE_ABCPERIOD             = { .Id = 1067, .Grouping = GROUP_SUNRISE, .Name = "AbcPeriod"};
const ValueIdentifier SUNRISE_ABCTARGET             = { .Id = 1068, .Grouping = GROUP_SUNRISE, .Name = "AbcTarget"};
const ValueIdentifier SUNRISE_IRFILTER              = { .Id = 1069, .Grouping = GROUP_SUNRISE, .Name = "IRFilter"};
const ValueIdentifier SUNRISE_METERCONTROL          = { .Id = 1070, .Grouping = GROUP_SUNRISE, .Name = "MeterControl"};

SenseairBase::SenseairBase()  {
    _valMaxPPM.i = 5000;
    _valBasePPM.i = 400;
    _valIsAbcEnabled.b = true;
    _valDaysPerAbcCycle.i = 1;

    AddValueSource(new ValueSource(*this,SUNRISE_MEASUREMENTINFO,       String, Dimensionless, _valMeasurementInfo, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SUNRISE_FIRMWARETYPE,          Int,    Dimensionless, _valFirmwareType, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SUNRISE_FIRMWARETEMPERATURE,   String, Dimensionless, _valFirmwareTemperature, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SUNRISE_ERROR,                 String, Dimensionless, _valError, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SUNRISE_SINGLEMEASUREMENTMODE, Bool,   Dimensionless, _valSingleMeasurementMode, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SUNRISE_MEASUREMENTPERIOD,     Int,    Dimensionless, _valMeasurementPeriod, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SUNRISE_NUMSAMPLES,            Int,    Dimensionless, _valMeasurementNoSamples, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SUNRISE_ABCPERIOD,             Int,    Dimensionless, _valAbcPeriod, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SUNRISE_ABCTARGET,             Int,    Dimensionless, _valAbcTarget, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SUNRISE_IRFILTER,              Int,    Dimensionless, _valFilter, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,SUNRISE_METERCONTROL,          String, Dimensionless, _valMeterControl, GET_LATEST_DATA));
}

#define SENSOR_STATUS_MSB_LOW_VOLTAGE_ERROR 1
#define SENSOR_STATUS_MSB_MEASUREMENT_TIMEOUT_ERROR 2
#define SENSOR_STATUS_LSB_FATAL_ERROR 1
#define SENSOR_STATUS_LSB_I2C_ERROR 2
#define SENSOR_STATUS_LSB_ALGORITHM_ERROR 4
#define SENSOR_STATUS_LSB_CALIBRATION_ERROR 8
#define SENSOR_STATUS_LSB_SELF_DIAGNOSTICS_ERROR 16
#define SENSOR_STATUS_LSB_OUT_OF_RANGE_ERROR 32
#define SENSOR_STATUS_LSB_MEMORY_ERROR 64
#define SENSOR_STATUS_LSB_NO_MEASUREMENT_COMPLETED 128


 void SenseairBase::UpdateErrorStatus(uint8_t* pInput) {
    _valHasError.b = false;
    _error = "";
    if(pInput[0] & SENSOR_STATUS_MSB_LOW_VOLTAGE_ERROR) {
        _valHasError.b= true;
        _error+="'Low Voltage' ";        
    } 

    if(pInput[0] & SENSOR_STATUS_MSB_MEASUREMENT_TIMEOUT_ERROR)  {
        _valHasError.b = true;
        _error+="'Measurement Timeout' ";    
    }


    if(pInput[1] & SENSOR_STATUS_LSB_FATAL_ERROR)  {
        _valHasError.b= true;
        _error+="Fatal ";    
    }

    if(pInput[1] & SENSOR_STATUS_LSB_I2C_ERROR)  {
        _valHasError.b = true;
        _error+="I2C ";    
    }

    if(pInput[1] & SENSOR_STATUS_LSB_ALGORITHM_ERROR)  {
         _valHasError.b = true;
         _error+="Algorithm ";    
    } 

    if(pInput[1] & SENSOR_STATUS_LSB_CALIBRATION_ERROR)  {
        _valHasError.b = true;
         _error+="Calibration ";    
    } 

    if(pInput[1] & SENSOR_STATUS_LSB_SELF_DIAGNOSTICS_ERROR)  {
        _valHasError.b = true;
        _error+="'Self Diagnostics' ";    
    } 

    if(pInput[1] & SENSOR_STATUS_LSB_OUT_OF_RANGE_ERROR)  {
        _valHasError.b = true;
        _error+="'Out of Range' ";    
    } 

    if(pInput[1] & SENSOR_STATUS_LSB_MEMORY_ERROR)  {
        _valHasError.b = true;
         _error+="'Memory' ";    
    } 
    
    _valIsHeatingUp.b = pInput[1] & SENSOR_STATUS_LSB_NO_MEASUREMENT_COMPLETED;

}
