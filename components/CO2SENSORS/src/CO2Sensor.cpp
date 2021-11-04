#include "CO2Sensor.h"

void CO2Sensor::RegisterValue(CO2SensorValue* pValue) {
    _values.push_back(pValue);
}

void CO2Sensor::RegisterAction(CO2SensorAction* pAction) {
    _actions.push_back(pAction);
}

std::vector<CO2SensorValue*>& CO2Sensor::GetValues() {
    return _values;
}

std::vector<CO2SensorAction*>& CO2Sensor::GetActions() {
    return _actions;
}

int CO2Sensor::GetMinBasePPM() {
    return 350;
}
int CO2Sensor::GetMaxBasePPM() {
    return 2000;
}
int CO2Sensor::GetMinDaysPerABCCycle()  {
    return 1;
}
int CO2Sensor::GetMaxDaysPerABCCycle() {
    return 31;
}

int CO2Sensor::GetRequiredManualCalibrationWaitInSeconds() {
    return 2*60;
}