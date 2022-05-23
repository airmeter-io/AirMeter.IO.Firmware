#pragma once

#include "BME280.h"
#include "CO2Sensor.h"

class ValueModel {
public:
    BME280SensorValues Bme280;
    CO2Sensor* CO2;
};