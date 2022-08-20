#!/bin/sh

./build-esp8266-device-partition.sh
python "$IDF_PATH/components/esptool_py/esptool/esptool.py" --chip esp8266 --port $1 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB 0x2F6000 build/dev.bin
