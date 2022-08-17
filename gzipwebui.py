import gzip
import shutil


with open('webui/dist/index.html', 'rb') as f_in:
    with gzip.open('webui/distgz/index.html.gz', 'wb') as f_out:
        shutil.copyfileobj(f_in, f_out)
with open('webui/dist/bundle.js', 'rb') as f_in:
    with gzip.open('webui/distgz/bundle.js.gz', 'wb') as f_out:
        shutil.copyfileobj(f_in, f_out)
with open('webui/dist/favicon.png', 'rb') as f_in:
    with gzip.open('webui/distgz/favicon.png.gz', 'wb') as f_out:
        shutil.copyfileobj(f_in, f_out)        