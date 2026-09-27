"""Gzip the built web UI (webui/dist) into webui/distgz.

webui/distgz is the source folder for the 'web' SPIFFS partition image
(see main/CMakeLists.txt). Run after `npm run build` in webui/.
"""
import gzip
import os
import shutil

SRC = "webui/dist"
DST = "webui/distgz"
FILES = ["index.html", "main.js", "favicon.png"]

os.makedirs(DST, exist_ok=True)
for name in FILES:
    src = os.path.join(SRC, name)
    dst = os.path.join(DST, name + ".gz")
    with open(src, "rb") as f_in, gzip.open(dst, "wb", compresslevel=9) as f_out:
        shutil.copyfileobj(f_in, f_out)
    print(f"{src} -> {dst} ({os.path.getsize(dst)} bytes)")
