- [ESP32 Device Parts & Build](#esp32-device-parts---build)
  * [Parts](#parts)
  * [Supported CO2 Sensors](#supported-co2-sensors)
  * [Build Instructions](#build-instructions)
    + [Using all UART on 3.3v Senseair Sunrise](#using-all-uart-on-33v-senseair-sunrise)
    + [Using all I2C on 3.3v Senseair Sunrise](#using-all-i2c-on-33v-senseair-sunrise)
    + [Using UART on 5v Winsen and Cubic sensors](#using-uart-on-5v-winsen-and-cubic-sensors)
- [Firmware Build Instructions](#firmware-build-instructions)
  * [Windows Environment Setup](#windows-environment-setup)
  * [Linux Environment Setup](#linux-environment-setup)
  * [Clone & initialise the latest OpenCO2 Monitor firmware](#clone---initialise-the-latest-openco2-monitor-firmware)
  * [Build the project](#build-the-project)
  * [Flash to device](#flash-to-device)

<small><i><a href='http://ecotrust-canada.github.io/markdown-toc/'>Table of contents generated with markdown-toc</a></i></small>


# ESP32 Device Parts & Build

This document provides build instructions for building a development set using an ESP32-WROOM32 board that illustrates the use of the Open CO2 Monitor firmware with ESP32 MCUs.

The options available to makers and producers in assembling devices based on the core components used in these units is vast. 

## Parts

|Part|Count|Description |
|--|--|--|
|ESP32-WROOM-32| 1 |This can be replaced with any similar ESP32-WROOM-32 based board, whilst not yet verified all members of the ESP32 family with enough flash should be suitable. Note that the firmware assumes 4mb of flash. |
|BME280|1|This sensor provides temperature, pressure and humidity readings. It can be substituted with a GY-BMP280 if required. The BME280 is presently in short supply. |
|SSD1306|1|This is required to enable secure WiFi provisioning and is used to display live readings. |
|CO2 Sensor|1|The list of supported sensors is listed separately below. The sensors supported at present all connect using the same 5v/GND/RX/TX pins.|
|MB85RC256V (Optional)|1|A FRAM module for persistent data staging before writing to flash. |
|Mini Breadboard|2|This is suggested for first builds. An alternative such as a PCB or just jumper leads will surfice. This guide assumes breadboards are being used. |

## Supported CO2 Sensors

|Model|Notes |
|--|--|
|Senseair Sunrise I2C|Supported and tested|
|Winsen MH-Z19B|Supported and tested|
|Winsen MH-Z19C|Supported and tested|
|Winsen MH-Z14|Should work but not tested|
|Cubic CM1107|Supported and tested|
|Cubic CM1106|Should work but not tested|

## Build Instructions

There are two designs described in this document which one to use depends on which CO2 sensor you are using. The designs are explained in using the steps to be followed

if building on a breadboard. Clearly as long as the same connections are maintained these can be built using a PCB or directly soldered wires.

Both designs utilise a power rail (+/-) for power distribution at 3.3V and another power rail as an I2C bus.

### Using all UART on 3.3v Senseair Sunrise

All components in this build using 3.3V so so first connect a 3.3v (+) pin and ground (-) pin of the ESP32-WROOM32 to a power rail +/- pair on the breadboard. Then proceed to connect the matching VCC (+) and ground (-) pins on both the BME280, SSD1306 display, MB85RC256V FRAM (optional) and Senseair Sunrise to the matching power rail in the pair just connected to the ESP32-WROOM32.

Next nominate another different power rail pair as an I2C bus. Connect D22 (SCL) on the ESP32-WROOM32 to the first rail in the pair and then connect D21 (SDA) to the second rail in the pair. Then for the BME280, SSD1306 display and MB85RC256V FRAM (optional) connect the matching SCL and SDA pins to the matching rails you just connected the corresponding pins on the ESP32-WROOM32.

Finally connect the RX pin on the CO2 sensor to RX2 on the ESP32-WROOM32 and the TX pin on the CO2 sensor to TX2 on the ESP32-WROOM32.

Do NOT connect the VV (5v from USB) pin on the ESP32-WROOM32 to anything.

![Schematic of a this built using two mini breadboards](https://i.ibb.co/B2pkZxP/breadboard-sunrise.png)

### Using all I2C on 3.3v Senseair Sunrise

All components in this build using 3.3V so so first connect a 3.3v (+) pin and ground (-) pin of the ESP32-WROOM32 to a power rail +/- pair on the breadboard. Then proceed to connect the matching VCC (+) and ground (-) pins on both the BME280, SSD1306 display, MB85RC256V FRAM (optional) and Senseair Sunrise to the matching power rail in the pair just connected to the ESP32-WROOM32.

Next nominate another different power rail pair as an I2C bus. Connect D22 (SCL) on the ESP32-WROOM32 to the first rail in the pair and then connect D21 (SDA) to the second rail in the pair. Then for the BME280, SSD1306 display, MB85RC256V FRAM (optional) and Senseair Sunrise connect the matching SCL and SDA pins to the matching rails you just connected the corresponding pins on the ESP32-WROOM32.

Do NOT connect the VV (5v from USB) pin on the ESP32-WROOM32 to anything.

![Schematic of a this built using two mini breadboards](https://i.ibb.co/B2pkZxP/breadboard-sunrise.png)

### Using UART on 5v Winsen and Cubic sensors

The SSD1306 display, MB85RC256V FRAM (optional) and the BME280 sensor both use I2C and operate at 3.3v. So first connect a 3.3v (+) pin and a ground (-) pin of the ESP32-WROOM32 to a power rail (+/- pair) on the breadboard. Then proceed to connect the matching VCC (+) and ground (-) pins on both the BME280 and SSD1306 display to the matching power rail in the pair just connected to the ESP32-WROOM32.

Use another different power rail pair as an I2C bus. Connect D22 (SCL) on the ESP32-WROOM32 to the first rail in the pair and then connect D21 (SDA) to the second rail in the pair. Then for both the BME280, MB85RC256V FRAM (optional) and the SSD1306 connect the match SCL and SDA pins to the matching rail you just connected the ESP32-WROOM32.

Connect the VV (5v from USB) pin on the ESP32-WROOM32 to the VCC (5v) pin on the CO2 sensor and connect the GND pin on the sensor to a GND pin on the ESP32-WROOM32.

Finally connect the RX pin on the CO2 sensor to RX2 on the ESP32-WROOM32 and the TX pin on the CO2 sensor to TX2 on the ESP32-WROOM32.

![Schematic of a this built using two mini breadboards](https://i.ibb.co/hVVGJqX/breadboard.png)


# Firmware Build Instructions

The project is built for the ESP32 platform using the latest v4.3 ESP32 IDF SDK. 


## Windows Environment Setup

Espressif provide a Windows package which ensures all the required components are installed. This is the easist way to setup a Windows build environment, the install can be found at [https://dl.espressif.com/dl/esp-idf/?idf=4.4](https://dl.espressif.com/dl/esp-idf/?idf=4.4). Install the v5.0 sdk.

More indepth instructions and alternative manual installation methods can be found at [https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html).

The remaining instructions in this document assume that the "ESP-IDF CMD" link on the start menu is being used as the CLI to enter commands.

## Linux Environment Setup

Follow the instructions at [https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) and ensure that the user enviroment is set up correctly.

The remaining instructions in this document assume that the users shell has the appropriate enviroment variables set and the IDF v4.3 SDK checked out.
  

## Clone & initialise the latest OpenCO2 Monitor firmware


Clone the repository & change into the directory.

    git clone https://github.com/FurballTheGreat/OpenCO2Monitor.git
    cd OpenCO2Monitor

  
Initialise  the GIT Submodules.

    git submodule init
    git submodule update

Copy the ESP32 SDK config files to the project root director.

On Linux type:-

    cp boards/esp32-idf/sdkconfig* .


On Windows type:-

    copy boards\esp32-idf\sdkconfig*.* .




## Build the project

Navigate in your terminal to the root folder of the OpenCO2 Monitor.

    cd ~/co2build/OpenCO2Monitor

Type the following to build the project.

    idf.py build


  
## Flash to device

On Linux type:

    idf.py flash /dev/ttsyusb0

Replace /dev/ttsyusb0 with the com port device exposed by the ESP32-WROOM32 board.

On Windows type:

    idf.py flash COM4

Replace COM4 with the correct COM port exposed by the ESP32-WROOM32 board.

