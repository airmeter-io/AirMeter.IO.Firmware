#pragma once

#include<string>
#include "ValueController.h"

const std::string GROUP_CO2 = "Co2";
const ValueIdentifier CO2VALUE =                          { .Id = 100, .Grouping = GROUP_CO2, .Name = "PPM" };
const ValueIdentifier CO2VALUE_UNFILTERED =               { .Id = 101, .Grouping = GROUP_CO2, .Name = "NoFilterPPM"} ;
const ValueIdentifier CO2VALUE_UNCOMPENSATED =            { .Id = 102, .Grouping = GROUP_CO2, .Name = "UncompPPM"};
const ValueIdentifier CO2VALUE_UNFILTERED_UNCOMPENSATED = { .Id = 103, .Grouping = GROUP_CO2, .Name = "NoFilterUncompPPM"};
const ValueIdentifier CO2VALUE_MAXPPM =                   { .Id = 104, .Grouping = GROUP_CO2, .Name = "MaxPPM"};
const ValueIdentifier CO2VALUE_BASEPPM =                  { .Id = 105, .Grouping = GROUP_CO2, .Name = "BasePPM"};
const ValueIdentifier CO2VALUE_MINBASEPPM =               { .Id = 106, .Grouping = GROUP_CO2, .Name = "MinBasePPM"};
const ValueIdentifier CO2VALUE_MAXBASEPPM =               { .Id = 107, .Grouping = GROUP_CO2, .Name = "MaxBasePPM"};
const ValueIdentifier CO2VALUE_HASERROR =                 { .Id = 108, .Grouping = GROUP_CO2, .Name = "HasError"};
const ValueIdentifier CO2VALUE_ISHEATING =                { .Id = 109, .Grouping = GROUP_CO2, .Name = "IsHeating"};
const ValueIdentifier CO2VALUE_ISABCENABLED =             { .Id = 110, .Grouping = GROUP_CO2, .Name = "IsABCEnabled"};
const ValueIdentifier CO2VALUE_HOURSPERCYCLE =            { .Id = 111, .Grouping = GROUP_CO2, .Name = "ABCHoursPerCycle"};
const ValueIdentifier CO2VALUE_MINHOURSPERCYCLE =         { .Id = 112, .Grouping = GROUP_CO2, .Name = "ABCMinHoursPerCycle"};
const ValueIdentifier CO2VALUE_MAXHOURSPERCYCLE =         { .Id = 113, .Grouping = GROUP_CO2, .Name = "ABCMaxHoursPerCycle"};
const ValueIdentifier CO2VALUE_STEPHOURSPERCYCLE =        { .Id = 113, .Grouping = GROUP_CO2, .Name = "ABCStepHoursPerCycle"};
const ValueIdentifier CO2VALUE_CALIBWAITTIME =            { .Id = 114, .Grouping = GROUP_CO2, .Name = "CalibWaitTime"};
const ValueIdentifier CO2VALUE_DEVICENAME =               { .Id = 115, .Grouping = GROUP_CO2, .Name = "DeviceName"};
const ValueIdentifier CO2VALUE_SERIALNO =                 { .Id = 116, .Grouping = GROUP_CO2, .Name = "SerialNo"};
const ValueIdentifier CO2VALUE_SOFWAREVERSION =           { .Id = 117, .Grouping = GROUP_CO2, .Name = "SoftwareVersion"};


const std::string GROUP_ATMOSPHERE = "Atmosphere";
const ValueIdentifier TEMPERATURE = { .Id = 200, .Grouping = GROUP_ATMOSPHERE, .Name = "Temp" };
const ValueIdentifier HUMIDITY =    { .Id = 201, .Grouping = GROUP_ATMOSPHERE, .Name = "Humidity" };
const ValueIdentifier PRESSURE =    { .Id = 202, .Grouping = GROUP_ATMOSPHERE, .Name = "Pressure" };
