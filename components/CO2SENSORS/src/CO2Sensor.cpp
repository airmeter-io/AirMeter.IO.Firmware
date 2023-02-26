#include "CO2Sensor.h"

CO2Sensor::CO2Sensor() {
   _valDeviceName.s = &_deviceName;
   _valSerialNo.s = &_serialNo;
   _valSwVersion.s = &_softwareVersion;
   AddValueSource(new ValueSource(*this,CO2VALUE_DEVICENAME,               String, Dimensionless, _valDeviceName, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this,CO2VALUE_SERIALNO,                 String, Dimensionless, _valSerialNo, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this,CO2VALUE_SOFWAREVERSION,           String, Dimensionless, _valSwVersion, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this,CO2VALUE,                          Int, PPM, _valCo2, DEFAULT_DATALOG | DEFAULT_MQTT | GET_LATEST_DATA | CALIBRATION_INFO));
   AddValueSource(new ValueSource(*this,CO2VALUE_UNFILTERED,               Int, PPM, _valCo2Unfiltered, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this,CO2VALUE_UNCOMPENSATED,            Int, PPM, _valCo2Uncompensated, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this,CO2VALUE_UNFILTERED_UNCOMPENSATED, Int, PPM, _valCo2UnfilteredUncompensated, GET_LATEST_DATA));       
   AddValueSource(new ValueSource(*this,CO2VALUE_MAXPPM,                   Int, PPM, _valMaxPPM, GET_LATEST_DATA));       
   AddValueSource(new ValueSource(*this,CO2VALUE_BASEPPM,                  Int, PPM, _valBasePPM, GET_LATEST_DATA | CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_MAXBASEPPM,               Int, PPM, _valMaxBasePPM, CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_MINBASEPPM,               Int, PPM, _valMinBasePPM, CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_HASERROR,                 Bool, Dimensionless, _valHasError, GET_LATEST_DATA));       
   AddValueSource(new ValueSource(*this,CO2VALUE_ISHEATING,                Bool, Dimensionless, _valIsHeatingUp, GET_LATEST_DATA));       
   AddValueSource(new ValueSource(*this,CO2VALUE_ISABCENABLED,             Bool, Dimensionless, _valIsAbcEnabled, GET_LATEST_DATA | CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_HOURSPERCYCLE,            Int, Days, _valHoursPerAbcCycle, GET_LATEST_DATA | CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_MINHOURSPERCYCLE,         Int, Days, _valMinHoursPerAbcCycle, CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_MAXHOURSPERCYCLE,         Int, Days, _valMaxHoursPerAbcCycle, CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_STEPHOURSPERCYCLE,        Int, Days, _valStepHoursPerAbcCycle, CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_CALIBWAITTIME,            Int, Seconds, _valCalibWaitTime, CALIBRATION_INFO));       


   AddMethodSource(new MethodSource(MediumAccessLevel, *this, GROUP_CO2, CO2METHOD_CALIBRATE, { Int }, [this]( const std::vector<Value>& pParams){ 
      ManualCalibration(pParams[0].i); 
   }));
   AddMethodSource(new MethodSource(MediumAccessLevel, *this, GROUP_CO2, CO2METHOD_DISABLEABC, { }, [this]( const std::vector<Value>& pParams){ 
      DisableABC(); 
   }));
   AddMethodSource(new MethodSource(MediumAccessLevel, *this, GROUP_CO2, CO2METHOD_ENABLEABC, { Int, Int }, [this]( const std::vector<Value>& pParams){ 
      EnableABC(pParams[0].i, pParams[1].i);
   }));
}
