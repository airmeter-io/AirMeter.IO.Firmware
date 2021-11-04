#!/bin/sh
mkdir -p build/webui
gzip -k webui/dist/index.html  -f
gzip -k webui/dist/bundle.js  -f
gzip -k webui/dist/favicon.png  -f
cp -f webui/dist/*.gz build/webui
python tools/spiffsgen.py --aligned-obj-ix-tables 0xC8000 build/webui "build/web.bin"
