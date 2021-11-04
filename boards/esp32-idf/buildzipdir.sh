#!/bin/sh

mkdir build/esp32zip
cp ../../opt/esp/idf/components/esptool_py/esptool/esptool.py build/esp32zip
cp build/bootloader/bootloader.bin build/esp32zip
cp build/partition_table/partition-table.bin build/esp32zip
cp build/ota_data_initial.bin build/esp32zip
cp build/main.bin build/esp32zip
cp build/web.bin build/esp32zip
cp boards/esp32-idf/flash* boards/esp32-idf/readme* build/esp32zip
cp LICENSE boards/esp32zip
cp webui/dist/bundle.js.LICENSE.txt boards/esp32zip
zip build/esp32build.zip boards/esp32zip/*


