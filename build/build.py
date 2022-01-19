#!/usr/bin/python3.8

"""
Usage:

Build and deploy a production release.
./build

Build a production release without deploying files to AWS.
./build -no-deploy
"""

import hashlib
import logging
import os
import pathlib
import shutil
import subprocess
import sys
import time
import typing
from urllib.request import urlopen

NO_DEPLOY = '--no-deploy'

DEBUG_BUILD  = False
RELEASE_SUFFIX = 'release'
DEBUG_SUFFIX = 'debug'
BUILD_SUFFIX = DEBUG_SUFFIX if DEBUG_BUILD else RELEASE_SUFFIX

LIB_DIR = pathlib.Path(__file__).absolute().parent / 'lib'
EMSCRIPTEN_DIR = pathlib.Path('/mnt/s/emscripten/emsdk')
BUILD_DIR = pathlib.Path('/mnt/s/canonic/build-canonic-wasm-debug' if DEBUG_BUILD else '/mnt/s/canonic/build-canonic-wasm')
OUTPUT_DIR = BUILD_DIR / BUILD_SUFFIX
CANONIC_DIR = pathlib.Path('/mnt/s/canonic')
QMAKE_PATH = pathlib.Path('/mnt/s/qt/build/qtbase/bin/qmake')
CANONIC_SRC_DIR = pathlib.Path(__file__).absolute().parent.parent
PROJECT_PATH = CANONIC_SRC_DIR / 'canonic.pro'

INDEX_HTML_FILE_NAME = 'index.html'
QT_LOADER_JS_FILE_NAME = 'qtloader.js'

AWS_BUCKET_NAME = 'app.canonic.com'
AWS_CF_DISTRIBUTION_ID = 'ED71MFRDRQ0UE'

SITEMAP_FILE_NAME = 'sitemap.xml'
WASM_BUILD_NAME = 'canonic.wasm'
JS_BUILD_NAME = 'canonic.js'
DEV_HTTPS_KEY_FILE = CANONIC_SRC_DIR / 'build/key.qml'
DEV_HTTPS_CERT_FILE = CANONIC_SRC_DIR / 'build/cert.qml'

JS_FILE_MD5_HASH_RELEASE_MARKER = 'var JS_FILE_MD5_HASH_RELEASE = "'
JS_FILE_MD5_HASH_DEBUG_MARKER = 'var JS_FILE_MD5_HASH_DEBUG = "'
WASM_FILE_MD5_HASH_RELEASE_MARKER = 'var WASM_FILE_MD5_HASH_RELEASE = "'
WASM_FILE_MD5_HASH_DEBUG_MARKER = 'var WASM_FILE_MD5_HASH_DEBUG = "'

JS_FILE_MD5_HASH_RELEASE_REPLACEMENT = 'var JS_FILE_MD5_HASH_RELEASE = "{}";'
JS_FILE_MD5_HASH_DEBUG_REPLACEMENT = 'var JS_FILE_MD5_HASH_DEBUG = "{}";'
WASM_FILE_MD5_HASH_RELEASE_REPLACEMENT = 'var WASM_FILE_MD5_HASH_RELEASE = "{}";'
WASM_FILE_MD5_HASH_DEBUG_REPLACEMENT = 'var WASM_FILE_MD5_HASH_DEBUG = "{}";'

if 'EMSDK' not in os.environ:
    os.chdir(EMSCRIPTEN_DIR)

    # Todo - this did not work last time I tried (had to manually source emsdk). This needs to be done is a bash file before hand
    #subprocess.run(['source', './emsdk_env.sh'])
    print("Emscripten not setup.")
    sys.exit()

if str(LIB_DIR) not in sys.path:
    sys.path.insert(0, str(LIB_DIR))

import boto3
from botocore.exceptions import ClientError

def upload_file(file_name, bucket, object_name=None, extra_args = None):
    """Upload a file to an S3 bucket

    :param file_name: File to upload
    :param bucket: Bucket to upload to
    :param object_name: S3 object name. If not specified then file_name is used
    :return: True if file was uploaded, else False
    """

    print("Uploading '{}'".format(file_name))

    # If S3 object_name was not specified, use file_name
    if object_name is None:
        object_name = file_name

    # Upload the file
    s3_client = boto3.client('s3')
    try:
        response = s3_client.upload_file(file_name, bucket, object_name, ExtraArgs=extra_args)
    except ClientError as e:
        logging.error(e)
        return False
    return True

# Create CloudFront invalidation
def create_invalidation(invalidations: typing.List[str], distribution_id: str):

    cloudfront_client = boto3.client('cloudfront')

    res = cloudfront_client.create_invalidation(
        DistributionId = distribution_id,
        InvalidationBatch={
            'Paths': {
                'Quantity': len(invalidations),
                'Items': invalidations
            },
            'CallerReference': str(time.time()).replace(".", "")
        }
    )

    invalidation_id = res['Invalidation']['Id']
    return invalidation_id


def TimestampISO8601(t):
  """Seconds since epoch (1970-01-01) --> ISO 8601 time string."""
  return time.strftime('%Y-%m-%dT%H:%M:%SZ', time.gmtime(t))


def inplace_replace(filename: str, old_strings: typing.List[str], new_strings: typing.List[str]) -> None:
    """ Replace a string occurrence in a file inplace."""

    if len(old_strings) != len(new_strings):
        raise ValueError("string lists must be the same length")

    with open(filename) as f:
        contents = f.read()

    with open(filename, 'w') as f:

        for i, old_string in enumerate(old_strings):
            contents = contents.replace(old_string, new_strings[i])

        f.write(contents)


def get_old_hashes() -> typing.Tuple[str, str, str, str]:
    old_index_html_file = str(urlopen("http://{}/{}".format(AWS_BUCKET_NAME, INDEX_HTML_FILE_NAME)).read(), 'utf8')

    i = old_index_html_file.index(JS_FILE_MD5_HASH_RELEASE_MARKER) + len(JS_FILE_MD5_HASH_RELEASE_MARKER)
    old_js_file_md5_release = old_index_html_file[i: old_index_html_file.index('"', i)]

    i = old_index_html_file.index(JS_FILE_MD5_HASH_DEBUG_MARKER) + len(JS_FILE_MD5_HASH_DEBUG_MARKER)
    old_js_file_md5_debug = old_index_html_file[i: old_index_html_file.index('"', i)]

    i = old_index_html_file.index(WASM_FILE_MD5_HASH_RELEASE_MARKER) + len(WASM_FILE_MD5_HASH_RELEASE_MARKER)
    old_wasm_file_md5_release = old_index_html_file[i: old_index_html_file.index('"', i)]

    i = old_index_html_file.index(WASM_FILE_MD5_HASH_DEBUG_MARKER) + len(WASM_FILE_MD5_HASH_DEBUG_MARKER)
    old_wasm_file_md5_debug = old_index_html_file[i: old_index_html_file.index('"', i)]

    return old_js_file_md5_release, old_js_file_md5_debug, old_wasm_file_md5_release, old_wasm_file_md5_debug


if BUILD_DIR.exists():
    print("Cleaning '{}'".format(BUILD_DIR))
    shutil.rmtree(BUILD_DIR)

print("Creating '{}'".format(str(BUILD_DIR)))
BUILD_DIR.mkdir(exist_ok=False)

os.chdir(BUILD_DIR)

if BUILD_SUFFIX == RELEASE_SUFFIX:
    result = subprocess.run([QMAKE_PATH.as_posix(), PROJECT_PATH, "CONFIG-=debug", "CONFIG+=release", "CONFIG+=wasm"])
elif BUILD_SUFFIX == DEBUG_SUFFIX:
    result = subprocess.run([QMAKE_PATH.as_posix(), PROJECT_PATH, "CONFIG+=debug", "CONFIG-=release", "CONFIG+=wasm"])
else:
    assert False

result = subprocess.run(['make', BUILD_SUFFIX])

# Copy index.html to the build directory
index_html_build_path = pathlib.Path(OUTPUT_DIR / INDEX_HTML_FILE_NAME)
index_html_build_path.unlink(missing_ok=True)
index_html_src_path = pathlib.Path(CANONIC_SRC_DIR / INDEX_HTML_FILE_NAME)
print("Copying '{}' to '{}'".format(index_html_src_path.as_posix(), index_html_build_path.as_posix()))
shutil.copy(index_html_src_path, index_html_build_path)

# Copy modified qtloader.js to build directory
qt_loader_js_build_path = pathlib.Path(OUTPUT_DIR / QT_LOADER_JS_FILE_NAME)
qt_loader_js_build_path.unlink(missing_ok=True)
qt_loader_js_src_path = pathlib.Path(CANONIC_SRC_DIR / QT_LOADER_JS_FILE_NAME)
print("Copying '{}' to '{}'".format(qt_loader_js_src_path.as_posix(), qt_loader_js_build_path.as_posix()))
shutil.copy(qt_loader_js_src_path, qt_loader_js_build_path)

os.chdir(OUTPUT_DIR)

# Calculate the md5 hash of the wasm file
wasm_file_md5 = hashlib.md5(open(OUTPUT_DIR / WASM_BUILD_NAME,'rb').read()).hexdigest()
wasm_file_name = 'canonic.{}.{}.wasm'.format(BUILD_SUFFIX, wasm_file_md5)

# Generate sitemap.xml file
with open(SITEMAP_FILE_NAME, 'w') as f:
    f.write(f"""<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
    <url>
        <loc>https://app.canonic.com/</loc>
        <lastmod>{TimestampISO8601(time.time())}</lastmod>
        <priority>1.0</priority>
    </url>
</urlset>
""")

if NO_DEPLOY in sys.argv:
    print("renaming '{}' to '{}'".format(WASM_BUILD_NAME, wasm_file_name))
    pathlib.Path(WASM_BUILD_NAME).rename(wasm_file_name)
else:
    print("Compressing canonic.wasm via brotli to {}".format(wasm_file_name))
    result = subprocess.run(['brotli', WASM_BUILD_NAME, '-o', wasm_file_name])

# Calculate the md5 hash of the js file
js_file_md5 = hashlib.md5(open(OUTPUT_DIR / JS_BUILD_NAME, 'rb').read()).hexdigest()

# Copy the js file to the output dir
js_file_name = 'canonic.{}.{}.js'.format(BUILD_SUFFIX, js_file_md5)
print("copying '{}' to '{}'".format(JS_BUILD_NAME, js_file_name))
shutil.copy(OUTPUT_DIR / JS_BUILD_NAME, OUTPUT_DIR / js_file_name)

# Update the index.html file with the new hashes
old_js_file_md5_release, old_js_file_md5_debug, old_wasm_file_md5_release, old_wasm_file_md5_debug = get_old_hashes()
inplace_replace(index_html_build_path,
                [
                    JS_FILE_MD5_HASH_RELEASE_REPLACEMENT.format(''),
                    JS_FILE_MD5_HASH_DEBUG_REPLACEMENT.format(''),
                    WASM_FILE_MD5_HASH_RELEASE_REPLACEMENT.format(''),
                    WASM_FILE_MD5_HASH_DEBUG_REPLACEMENT.format('')
                ],
                [
                    JS_FILE_MD5_HASH_RELEASE_REPLACEMENT.format(old_js_file_md5_release if DEBUG_BUILD else js_file_md5),
                    JS_FILE_MD5_HASH_DEBUG_REPLACEMENT.format(js_file_md5 if DEBUG_BUILD else old_js_file_md5_debug),
                    WASM_FILE_MD5_HASH_RELEASE_REPLACEMENT.format(old_wasm_file_md5_release if DEBUG_BUILD else wasm_file_md5),
                    WASM_FILE_MD5_HASH_DEBUG_REPLACEMENT.format(wasm_file_md5 if DEBUG_BUILD else old_wasm_file_md5_debug)
                ])

if NO_DEPLOY not in sys.argv:
    os.chdir(OUTPUT_DIR)

    print("Starting file uploads")

    upload_file(INDEX_HTML_FILE_NAME, AWS_BUCKET_NAME, extra_args={
        'ACL': 'public-read',
        'ContentType': 'text/html',
    })

    upload_file(SITEMAP_FILE_NAME, AWS_BUCKET_NAME, extra_args={
            'ACL': 'public-read',
            'ContentType': 'text/xml',
        })

    upload_file('qtloader.js', AWS_BUCKET_NAME, extra_args={
            'ACL': 'public-read',
            'ContentType': 'application/javascript',
        })

    upload_file(js_file_name, AWS_BUCKET_NAME, extra_args={
            'ACL': 'public-read',
            'ContentType': 'application/javascript',
        })

    upload_file(wasm_file_name, AWS_BUCKET_NAME, extra_args={
            'ACL': 'public-read',
            'ContentEncoding': 'br',
            'ContentType': 'application/wasm',
            'Metadata': {
                'decompressedcontentlength': str(pathlib.Path('canonic.wasm').stat().st_size)
            }
        })

    print("Invalidating cloudfront content")
    invalidation_id = create_invalidation(['/{}'.format(SITEMAP_FILE_NAME),
                                           '/{}'.format(INDEX_HTML_FILE_NAME),
                                           '/qtloader.js'], AWS_CF_DISTRIBUTION_ID)

    print("invalidation_id:", invalidation_id)
