#!/bin/sh

mkdir /tmp/build
mkdir /tmp/build/esp8266zip
cp build/bootloader/bootloader.bin /tmp/build/esp8266zip
cp build/partition_table/partition-table.bin /tmp/build/esp8266zip
cp build/ota_data_initial.bin /tmp/build/esp8266zip
cp build/airmeter-io.bin /tmp/build/esp8266zip
cp build/web.bin /tmp/build/esp8266zip
cp boards/esp8266-rtossdk/flash* boards/esp8266-rtossdk/readme* /tmp/build/esp8266zip
cp LICENSE /tmp/build/esp8266zip
cp webui/dist/bundle.js.LICENSE.txt /tmp/build/esp8266zip
ls /tmp/build/esp8266zip
zip -j /tmp/build/ESP8266Binaries$GITHUB_REF_NAME.zip /tmp/build/esp8266zip/*


