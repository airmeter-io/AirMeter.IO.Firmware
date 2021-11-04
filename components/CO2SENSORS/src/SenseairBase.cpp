#include "SenseairBase.h"

SenseairBase::SenseairBase()  {
    _serialNo = "UNKNOWN";
    _softVer = "UNKNOWN";
    RegisterValue(&_valMeasurementInfo);
    RegisterValue(&_valFirmwareType);
    RegisterValue(&_valFirmwareTemperature);
    RegisterValue(&_valError);
  
    RegisterValue(&_valSingleMeasurementMode);
    RegisterValue(&_valMeasurementPeriod);
    RegisterValue(&_valMeasurementNoSamples);
    RegisterValue(&_valAbcPeriod);
    RegisterValue(&_valAbcTarget);
    RegisterValue(&_valFilter);
    RegisterValue(&_valMeterControl);
}

const std::vector<int> SenseairBase::availablePPMs = { 2000, 5000 };


std::string& SenseairBase::GetSerialNo() {
    return _serialNo;
}

std::string& SenseairBase::GetDeviceName() {
    return _deviceName;
}

std::string& SenseairBase::GetSWVersion() {
    return _softVer;
}


int SenseairBase::GetPPM() {
    return _ppm;
}

int SenseairBase::GetMaxPPM() {
    return 5000;
}

const std::vector<int>& SenseairBase::GetAvailableMaxPPM() const {
    return availablePPMs;
}
void SenseairBase::SetMaxPPM(int pMaxPPM) {

}
bool SenseairBase::GetIsHeatingUp() {
    return _isHeatingUp;
}

bool SenseairBase::GetHasError() {
    return _sensorError;
}

int SenseairBase::GetBasePPM() {
    return 400;
}

int SenseairBase::GetDaysPerABCCycle() {
    return 1;
}

bool SenseairBase::GetIsABCEnabled() {
    return _abcEnabled;
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
    _sensorError = false;
    _error = "";
    if(pInput[0] & SENSOR_STATUS_MSB_LOW_VOLTAGE_ERROR) {
        _sensorError= true;
        _error+="'Low Voltage' ";        
    } 

    if(pInput[0] & SENSOR_STATUS_MSB_MEASUREMENT_TIMEOUT_ERROR)  {
        _sensorError = true;
        _error+="'Measurement Timeout' ";    
    }


    if(pInput[1] & SENSOR_STATUS_LSB_FATAL_ERROR)  {
        _sensorError= true;
        _error+="Fatal ";    
    }

    if(pInput[1] & SENSOR_STATUS_LSB_I2C_ERROR)  {
        _sensorError = true;
        _error+="I2C ";    
    }

    if(pInput[1] & SENSOR_STATUS_LSB_ALGORITHM_ERROR)  {
         _sensorError = true;
         _error+="Algorithm ";    
    } 

    if(pInput[1] & SENSOR_STATUS_LSB_CALIBRATION_ERROR)  {
        _sensorError = true;
         _error+="Calibration ";    
    } 

    if(pInput[1] & SENSOR_STATUS_LSB_SELF_DIAGNOSTICS_ERROR)  {
        _sensorError = true;
        _error+="'Self Diagnostics' ";    
    } 

    if(pInput[1] & SENSOR_STATUS_LSB_OUT_OF_RANGE_ERROR)  {
        _sensorError = true;
        _error+="'Out of Range' ";    
    } 

    if(pInput[1] & SENSOR_STATUS_LSB_MEMORY_ERROR)  {
        _sensorError = true;
         _error+="'Memory' ";    
    } 
    
    _isHeatingUp = pInput[1] & SENSOR_STATUS_LSB_NO_MEASUREMENT_COMPLETED;

}
