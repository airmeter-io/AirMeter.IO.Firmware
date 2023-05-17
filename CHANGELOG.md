
The update introduces a Material UI-based web interface and a "value system" for flexible data logging. It revamps WiFi management to support multiple networks and reconnections, adds support for the new Senseair Sunlight CO2 sensor, improves Sunrise implementation, and fixes various bugs.

# 0.3.0
- New Material UI based web interface.
- A generic "value system" that greatly increases the flexibility of what data can be logged and sent to the cloud.
- Rebuilt WiFI connection management to support multiple networks and reconnection.
- Support for the new Senseair Sunlight CO2 sensor & enhanced Sunrise implementation.
- The MQTT publishing feature now support mqtts and username/password authentication.
- Partition table has been tweaked to increase size of app partitions. 
- Various bug fixes.

# 0.2.1 - Portable ePaper Device (Beta + fixes)
- More stability in EPD updates.
- Buttons function better due to fix in GPIO handling. 

# 0.2.0 - Portable ePaper Device (Beta)
- Adds support for EPD displays with general infrastructure in place for SSD168x panels and specific support for DEPG0213BN.
- Adds advanced power management with less < 5mA idle connected to wifi.
- Adds support for single measurement mode and power management of Senseair Sunrise
- Adds screen manager, font infrastructure including utf8 & JSON configuration.
- Adds gpio manager & button manager to support interaction via device buttons.
- Adds a QR code initial setup procedure to quickly connect to the captive setup portal.

# 0.1.1 - Early development release (Obsolete)
# 0.1.0 - Early development release (Obsolete)
