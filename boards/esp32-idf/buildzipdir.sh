#!/bin/sh

mkdir /tmp/build
mkdir /tmp/build/esp32zip
cp ../../opt/esp/idf/components/esptool_py/esptool/esptool.py /tmp/build/esp32zip
cp build/bootloader/bootloader.bin /tmp/build/esp32zip
cp build/partition_table/partition-table.bin /tmp/build/esp32zip
cp build/ota_data_initial.bin /tmp/build/esp32zip
cp build/main.bin /tmp/build/esp32zip
cp build/web.bin /tmp/build/esp32zip
cp boards/esp32-idf/flash* boards/esp32-idf/readme* /tmp/build/esp32zip
cp LICENSE /tmp/build/esp32zip
cp webui/dist/bundle.js.LICENSE.txt /tmp/build/esp32zip
ls /tmp/build/esp32zip
zip /tmp/build/esp32build.zip /tmp/build/esp32zip/*


