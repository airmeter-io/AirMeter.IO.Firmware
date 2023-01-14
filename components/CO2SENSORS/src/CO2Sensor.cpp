#include "CO2Sensor.h"

CO2Sensor::CO2Sensor() {
   _valDeviceName.s = &_deviceName;
   _valSerialNo.s = &_serialNo;
   _valSwVersion.s = &_softwareVersion;
   AddValueSource(new ValueSource(*this,CO2VALUE_DEVICENAME,               Generic, String, Dimensionless, _valDeviceName, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this,CO2VALUE_SERIALNO,                 Generic, String, Dimensionless, _valSerialNo, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this,CO2VALUE_SOFWAREVERSION,           Generic, String, Dimensionless, _valSwVersion, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this,CO2VALUE,                          AtmosphericCO2, Int, PPM, _valCo2, DEFAULT_DATALOG | DEFAULT_MQTT));
   AddValueSource(new ValueSource(*this,CO2VALUE_UNFILTERED,               AtmosphericCO2, Int, PPM, _valCo2Unfiltered, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this,CO2VALUE_UNCOMPENSATED,            AtmosphericCO2, Int, PPM, _valCo2Uncompensated, GET_LATEST_DATA));
   AddValueSource(new ValueSource(*this,CO2VALUE_UNFILTERED_UNCOMPENSATED, AtmosphericCO2, Int, PPM, _valCo2UnfilteredUncompensated, GET_LATEST_DATA));       
   AddValueSource(new ValueSource(*this,CO2VALUE_MAXPPM,                   AtmosphericCO2, Int, PPM, _valMaxPPM, GET_LATEST_DATA));       
   AddValueSource(new ValueSource(*this,CO2VALUE_BASEPPM,                  AtmosphericCO2, Int, PPM, _valBasePPM, GET_LATEST_DATA | CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_MAXBASEPPM,               AtmosphericCO2, Int, PPM, _valMaxBasePPM, CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_MINBASEPPM,               AtmosphericCO2, Int, PPM, _valMinBasePPM, CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_HASERROR,                 Generic, Bool, Dimensionless, _valHasError, GET_LATEST_DATA));       
   AddValueSource(new ValueSource(*this,CO2VALUE_ISHEATING,                Generic, Bool, Dimensionless, _valIsHeatingUp, GET_LATEST_DATA));       
   AddValueSource(new ValueSource(*this,CO2VALUE_ISABCENABLED,             Generic, Bool, Dimensionless, _valIsAbcEnabled, GET_LATEST_DATA | CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_DAYSPERCYCLE,             Generic, Int, Days, _valDaysPerAbcCycle, GET_LATEST_DATA | CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_MINDAYSPERCYCLE,          Generic, Int, Days, _valMinDaysPerAbcCycle, CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_MAXDAYSPERCYCLE,          Generic, Int, Days, _valMaxDaysPerAbcCycle, CALIBRATION_INFO));       
   AddValueSource(new ValueSource(*this,CO2VALUE_CALIBWAITTIME,            Generic, Int, Seconds, _valCalibWaitTime, CALIBRATION_INFO));       


   AddMethodSource(new MethodSource(MediumAccessLevel, *this, CO2METHOD_GROUP, CO2METHOD_CALIBRATE, { Int }, [this]( const std::vector<Value>& pParams){ 
      ManualCalibration(pParams[0].i); 
   }));
   AddMethodSource(new MethodSource(MediumAccessLevel, *this, CO2METHOD_GROUP, CO2METHOD_DISABLEABC, { }, [this]( const std::vector<Value>& pParams){ 
      DisableABC(); 
   }));
   AddMethodSource(new MethodSource(MediumAccessLevel, *this, CO2METHOD_GROUP, CO2METHOD_ENABLEABC, { Int, Int }, [this]( const std::vector<Value>& pParams){ 
      EnableABC(pParams[0].i, pParams[1].i);
   }));
}
