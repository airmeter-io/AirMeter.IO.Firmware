
- [ESP8266 Device Parts & Build](#esp8266-device-darts---build)
- [Device Parts & Build](#device-parts---build)
  * [Parts](#parts)
  * [Supported CO2 Sensors](#supported-co2-sensors)
  * [Build Instructions](#build-instructions)
    + [Using all I2C (Senseair Sunrise)](#using-all-i2c--senseair-sunrise-)
    + [Using UART for CO2 Sensor (Winsen and Cubic sensors)](#using-uart-for-co2-sensor--winsen-and-cubic-sensors-)
- [Firmware Build Instructions](#firmware-build-instructions)
  * [Windows Requirements](#windows-requirements)
  * [Step 1 - Install Required Packages](#step-1---install-required-packages)
  * [Step 2 - Setup ESP8266 SDK and build folders](#step-2---setup-esp8266-sdk-and-build-folders)
  * [Step 3 - Clone & initialise the latest OpenCO2 Monitor firmware](#step-3---clone---initialise-the-latest-openco2-monitor-firmware)
  * [Step 4 - Build the project](#step-4---build-the-project)
  * [Step 5 - Flash to device](#step-5---flash-to-device)
    + [Windows (Ubuntu WSL2)](#windows--ubuntu-wsl2-)
    + [Linux](#linux)

# ESP8266 Device Parts & Build

This document provides build instructions for building a development set using an ESP8266 NodeMCU board to illustrate the use of the Open CO2 Monitor firmware on ESP8266 MCUs.

The options available to makers and producers in assembling devices based on the core components used in these units is vast. 

## Parts

|Part|Count|Description |
|--|--|--|
| NodeMCU v2/3| 1 |This can be replaced with any similar ESP8266EX based board. Note that the firmware assumes 4mb of flash. |
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

### Using all I2C (Senseair Sunrise)

All components in this build using 3.3V so so first connect a 3.3v (+) pin and ground (-) pin of the NodeMCU to a power rail +/- pair on the breadboard. Then proceed to connect the matching VCC (+) and ground (-) pins on both the BME280, SSD1306 display, MB85RC256V FRAM (optional) and Senseair Sunrise to the matching power rail in the pair just connected to the NodeMCU.

Next nominate another different power rail pair as an I2C bus. Connect D2 (SCL) on the NodeMCU to the first rail in the pair and then connect D1 (SDA) to the second rail in the pair. Then for the BME280, SSD1306 display, MB85RC256V FRAM (optional) and Senseair Sunrise connect the matching SCL and SDA pins to the matching rails you just connected the corresponding pins on the NodeMCU.

Do NOT connect the VV (5v from USB) pin on the NodeMCU to anything unlike the alternative build.

![Schematic of a this built using two mini breadboards](https://i.ibb.co/B2pkZxP/breadboard-sunrise.png)

### Using UART for CO2 Sensor (Winsen and Cubic sensors)

The SSD1306 display, MB85RC256V FRAM (optional) and the BME280 sensor both use I2C and operate at 3.3v. So first connect a 3.3v (+) pin and a ground (-) pin of the NodeMCU to a power rail (+/- pair) on the breadboard. Then proceed to connect the matching VCC (+) and ground (-) pins on both the BME280 and SSD1306 display to the matching power rail in the pair just connected to the NodeMCU.

Use another different power rail pair as an I2C bus. Connect D2 (SCL) on the NodeMCU to the first rail in the pair and then connect D1 (SDA) to the second rail in the pair. Then for both the BME280, MB85RC256V FRAM (optional) and the SSD1306 connect the match SCL and SDA pins to the matching rail you just connected the NodeMCU.

Connect the VV (5v from USB) pin on the NodeMCU to the VCC (5v) pin on the CO2 sensor and connect the GND pin on the sensor to a GND pin on the NodeMCU.

Finally connect the RX pin on the CO2 sensor to D8 on the NodeMCU and the TX pin on the CO2 sensor to D7 on the NodeMCU.

![Schematic of a this built using two mini breadboards](https://i.ibb.co/hVVGJqX/breadboard.png)


# Firmware Build Instructions

The project is built using the latest 3.4 ESP8266 RTOS SDK that uses an IDF style model. As such in due course porting to the ESP32 platform will be relatively straight forward.

Whilst any modern Linux distribution should be able to build this firmware Ubuntu is used as the distro throughout this guide.

## Windows Requirements

The recommended Windows build environment is using Ubuntu (WSL2) on Windows 10. Ubuntu & Python must be installed from the Windows Store and then the Ubuntu WSL2 VM must be initialised by accessing the "Ubuntu" Start Menu item.

Once Ubuntu is installed it is recommended that you navigate using Windows Explorer to \\wsl$ and map the Ubuntu VM as a network drive. This share can be used to access the source files using your favourite IDE.

Additionally Windows Terminal available in the Windows Store provides a vastly improved terminal experience on Windows and is recommended for use once Ubuntu is installed and initialised. At this point Windows Terminal will make the Ubuntu install available as one of the shell type short cuts.

## Step 1 - Install Required Packages

On Ubuntu the required packages can be installed with a single apt-get command. Note that python-is-python3 is required and may cause problems with other packages which rely on python2.7, this case is considered out of scope as python2.7 is deprecated.

    sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python3 python3-serial python3-pip python-is-python3

## Step 2 - Setup ESP8266 SDK and build folders

These steps can be run as a non-privileged user as all folders are created within that user's home directory.

First create the top-level folder which will contain all the required components.

  

    mkdir ~/co2build
    cd ~/co2build

Get the XTENSA LX106 tool chain and extract it.

    wget https://dl.espressif.com/dl/xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz
    tar -zxf xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz 

Clone the release/v3.4 branch of the ESP8266 RTOS SDK.

    git clone -b release/v3.4 https://github.com/espressif/ESP8266_RTOS_SDK.git
 
Set the IDF_PATH environment variable.

    echo export IDF_PATH=~/co2build/ESP8266_RTOS_SDK/ >> ~/.bashrc
    export IDF_PATH=~/co2build/ESP8266_RTOS_SDK/

Install the required Python packages

    python3 -m pip install --user -r $IDF_PATH/requirements.txt

  
  

## Step 3 - Clone & initialise the latest OpenCO2 Monitor firmware

  

Clone the repository & change into the directory.

    git clone https://github.com/FurballTheGreat/OpenCO2Monitor.git
    cd OpenCO2Monitor

  

Initialise  the GIT Submodules.

    git submodule init
    git submodule update

Copy the ESP8266 SDK config files.

    cp boards/esp8266-rtossdk/sdkconfig* .

## Step 4 - Build the project

Navigate in your terminal to the root folder of the OpenCO2 Monitor.

    cd ~/co2build/OpenCO2Monitor

Type "make" to build the project. E.g.

    furball@host:~/co2build/OpenCO2Monitor$ make
    Toolchain path: /home/furball/co2build/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
    Toolchain version: esp-2020r3-49-gd5524c1
    Compiler version: 8.4.0
    Python requirements from /home/furball/co2build/ESP8266_RTOS_SDK/requirements.txt are satisfied.
    App "open-co2-monitor" version: 1452f88
    To flash all build output, run 'make flash' or:
    python3 /home/furball/co2build/ESP8266_RTOS_SDK/components/esptool_py/esptool/esptool.py --chip esp8266 --port COM5 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB 0xd000 /home/furball/co2build/OpenCO2Monitor/build/ota_data_initial.bin 0x0 /home/furball/co2build/OpenCO2Monitor/build/bootloader/bootloader.bin 0x10000 /home/furball/co2build/OpenCO2Monitor/build/open-co2-monitor.bin 0x8000 /home/furball/co2build/OpenCO2Monitor/build/partitions.bin

  
## Step 5 - Flash to device

### Windows (Ubuntu WSL2)

Since WSL2 does not support COM port access one cannot use the built in make flash command. Using PowerShell navigate to the OpenCO2Monitor folders on the Ubuntu VM shared drive mapped earlier and run.

    .\FlashAll.bat COM5

Replace COM5 with the correct COM port.

### Linux

Connect your device to the computer using a USB cable. Identify the path of the serial port that this device exposes. The path will probably be /dev/ttyUSB[0-n]. The first time you flash you will need set the path to this serial port. This can be done by using the configuration menus which can be accessed by executing:

    make menuconfig

![make menuconfig step 1](https://i.ibb.co/j3pLVhK/Step1.png)

![make menuconfig step 2](https://i.ibb.co/RgTTJ3x/Step2.png)

  

Once the build configuration menu displays choose the "Serial flasher config" sub menu in which you will find the "Default Serial Port" option. Set this to the path to the serial port exposed by the device. Finally use the buttons to save and then exit the configuration menu.

Once the serial port configuration is correct you can flash the device using the following command:

    $ make flash
    Toolchain path: ~/esp/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
    Toolchain version: esp-2020r3-49-gd5524c1
    Compiler version: 8.4.0
    Python requirements from ~/esp/ESP8266_RTOS_SDK/requirements.txt are satisfied.
    App "factory-test" version: 1b8e7cd-dirty
    CC build/app_update/esp_app_desc.o
    AR build/app_update/libapp_update.a
    Generating esp8266.project.ld
    LD build/factory-test.elf
    esptool.py v2.4.0
    Flashing binaries to serial port /dev/ttyUSB5 (app at offset 0x10000)...
    esptool.py v2.4.0
    Connecting....
    Chip is ESP8266EX
    Features: WiFi
    MAC: cc:50:e3:ce:0c:4a
    Uploading stub...
    Running stub...
    Stub running...
    Configuring flash size...
    Compressed 8192 bytes to 31...
    Wrote 8192 bytes (31 compressed) at 0x0000d000 in 0.0 seconds (effective 5457.6 kbit/s)...
    Hash of data verified.
    Compressed 9872 bytes to 7054...
    Wrote 9872 bytes (7054 compressed) at 0x00000000 in 0.6 seconds (effective 125.8 kbit/s)...
    Hash of data verified.
    Compressed 805600 bytes to 522951...
    Wrote 805600 bytes (522951 compressed) at 0x00010000 in 46.2 seconds (effective 139.4 kbit/s)...
    Hash of data verified.
    Compressed 3072 bytes to 121...
    Wrote 3072 bytes (121 compressed) at 0x00008000 in 0.0 seconds (effective 1228.6 kbit/s)...
    Hash of data verified.
    Leaving...
    Hard resetting via RTS pin...

Finally the Web UI must be flashed to the device which can be achieved as follows:-

	chmod a+x flash-web-partition.sh
	./flash-web-partition.sh /dev/ttyUSB5