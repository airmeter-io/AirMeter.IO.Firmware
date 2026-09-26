#!/bin/sh
# Package an ESP32 build into the flashable zip published on GitHub releases
# and consumed by the web flash tool. Run from the repo root after `idf.py build`.
#
#   RELEASE_TAG  label used in the zip name and manifest (default: git describe)
#   OUT_DIR      where to write the zip (default: release)
set -e

RELEASE_TAG="${RELEASE_TAG:-$(git describe --tags --always --dirty)}"
OUT_DIR="${OUT_DIR:-release}"
STAGE="$OUT_DIR/esp32zip"
ZIP="$OUT_DIR/AirMeter.io-binary-esp32-$RELEASE_TAG.zip"

rm -rf "$STAGE"
mkdir -p "$STAGE"
cp build/bootloader/bootloader.bin \
   build/partition_table/partition-table.bin \
   build/ota_data_initial.bin \
   build/main.bin \
   build/web.bin \
   build/dev.bin \
   boards/esp32-idf/flash.sh \
   boards/esp32-idf/flash.bat \
   boards/esp32-idf/readme.txt \
   LICENSE \
   "$STAGE"
if [ -f webui/dist/main.js.LICENSE.txt ]; then
    cp webui/dist/main.js.LICENSE.txt "$STAGE/webui.LICENSE.txt"
fi
sed "s/RELEASETAG/$RELEASE_TAG/" boards/esp32-idf/manifest.json > "$STAGE/manifest.json"

ls -l "$STAGE"
python3 - "$ZIP" "$STAGE" <<'PY'
import os, sys, zipfile
zip_path, stage = sys.argv[1], sys.argv[2]
with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as z:
    for name in sorted(os.listdir(stage)):
        z.write(os.path.join(stage, name), name)
print("Created", zip_path, os.path.getsize(zip_path), "bytes")
PY
