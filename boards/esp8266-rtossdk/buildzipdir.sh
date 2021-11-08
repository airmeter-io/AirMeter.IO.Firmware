#!/bin/sh

mkdir /tmp/build
mkdir /tmp/build/esp8266zip
cp build/bootloader/bootloader.bin /tmp/build/esp8266zip
cp build/partition_table/partition-table.bin /tmp/build/esp8266zip
cp build/ota_data_initial.bin /tmp/build/esp8266zip
cp build/airmeter-io.bin /tmp/build/esp8266zip
cp build/web.bin /tmp/build/esp8266zip
cp boards/esp8266-rtossdk/flash* boards/esp8266-rtossdk/readme* /tmp/build/esp8266zip
sed 's/RELEASETAG/$GITHUB_REF_NAME/' boards/esp8266-rtossdk/manifest.json  > /tmp/build/esp8266zip/manifest.json
cp LICENSE /tmp/build/esp8266zip
cp webui/dist/bundle.js.LICENSE.txt /tmp/build/esp8266zip
ls /tmp/build/esp8266zip
zip -j /tmp/build/AirMeter.io-binary-esp8266-$GITHUB_REF_NAME.zip /tmp/build/esp8266zip/*


