#!/bin/sh
python -m esptool --chip esp8266 --port COM5 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB 0xd000 ota_data_initial.bin 0x0 bootloader/bootloader.bin 0x10000 airmeter-io.bin 0x8000 partitions.bin 0x22E000 web.bin 0x2F6000 dev.bin
