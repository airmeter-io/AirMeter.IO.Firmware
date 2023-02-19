import gzip
import shutil


with open('webui2/dist/index.html', 'rb') as f_in:
    with gzip.open('webui/distgz/index.html.gz', 'wb') as f_out:
        shutil.copyfileobj(f_in, f_out)
with open('webui2/dist/main.js', 'rb') as f_in:
    with gzip.open('webui/distgz/main.js.gz', 'wb') as f_out:
        shutil.copyfileobj(f_in, f_out)
with open('webui/dist/favicon.png', 'rb') as f_in:
    with gzip.open('webui/distgz/favicon.png.gz', 'wb') as f_out:
        shutil.copyfileobj(f_in, f_out)        