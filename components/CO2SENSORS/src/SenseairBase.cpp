#include "SenseairBase.h"

SenseairBase::SenseairBase()  {
    _valMaxPPM.i = 5000;
    _valBasePPM.i = 400;
    _valIsAbcEnabled.b = true;
    _valDaysPerAbcCycle.i = 1;

    AddValueSource(new ValueSource(*this,"Sunrise_MeasurementInfo",       Generic, String, Dimensionless, _valMeasurementInfo, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,"Sunrise_FirmwareType",          Generic, Int,    Dimensionless, _valFirmwareType, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,"Sunrise_FirmwareTemperature",   Generic, String, Dimensionless, _valFirmwareTemperature, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,"Sunrise_Error",                 Generic, String, Dimensionless, _valError, GET_LATEST_DATA));

    AddValueSource(new ValueSource(*this,"Sunrise_SingleMeasurementMode", Generic, Bool,   Dimensionless, _valSingleMeasurementMode, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,"Sunrise_MeasurementPeriod",     Generic, Int,    Dimensionless, _valMeasurementPeriod, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,"Sunrise_NumSamples",            Generic, Int,    Dimensionless, _valMeasurementNoSamples, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,"Sunrise_AbcPeriod",             Generic, Int,    Dimensionless, _valAbcPeriod, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,"Sunrise_AbcTarget",             Generic, Int,    Dimensionless, _valAbcTarget, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,"Sunrise_IRFilter",              Generic, Int,    Dimensionless, _valFilter, GET_LATEST_DATA));
    AddValueSource(new ValueSource(*this,"Sunrise_MeterControl",          Generic, String, Dimensionless, _valMeterControl, GET_LATEST_DATA));
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
