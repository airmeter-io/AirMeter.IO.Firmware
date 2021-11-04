## What is AirMeter.io?
AirMeter.io is an OpenSource platform for building Internet of Things (IoT) connected CO2 monitors.

![The AirMeter.io web interface](https://i.ibb.co/D9CSpRw/banner.png)

The responsive web interface guides the user through through calibration & configuration. The most recent readings are displayed using visually striking gauges and the on-device data logs are made available as charts. 

Up to six months of readings can be recorded in flash memory. Equally however an *AirMeter.io* device can be used as part of a cloud using the MQTT publishing feature. 

The on-device web interface is built using [ReactJS](https://reactjs.org), [Charts.js](https://www.chartjs.org/)  & [Canvas Gauges](https://canvas-gauges.com/) and simply invokes REST APIs made exposed by the firmware. As the packaged ReactJS app is served from a dedicated partition by the C++ core, it can be replaced entirely with any application written in JavaScript.


## Why Measure CO2?

It is now accepted by major public health agencies that COVID-19 can and does spread through aerosols. See [Centers for Disease Control and Prevention](https://www.cdc.gov/coronavirus/2019-ncov/science/science-briefs/sars-cov-2-transmission.html), [World Health Organisation (WHO)](https://www.who.int/news-room/q-a-detail/coronavirus-disease-covid-19-how-is-it-transmitted), [European Centre for Disease Prevention and Control (ECDC)](https://www.ecdc.europa.eu/en/covid-19/questions-answers/questions-answers-basic-facts).

The outside CO2 level away from combustion sources is relatively constant at ~415ppm. When a person exhales the release air containing 35,000-45,000ppm therefore the level of CO2 in an enclosed space is a *proxy* to the level of exhaled air which hasn't been ventilated.

If the CO2 level is too high then ventilation & filtration are the primary approaches to reducing the level of potentially dangerous aerosols. 

## More Information

This project relies directly on a number of open source projects, has learnt from other projects & individuals. The links below attempt a best as is possible to give full credit where it is due.
- [Open source projects directly used by *AirMeter.io*](https://airmeter.io/thirdparty-components)
- [Credits to individuals and projects who contributed their expertise](https://airmeter.io/credits)