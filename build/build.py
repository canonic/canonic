#!/usr/bin/python3.8
import logging
import os
import pathlib
import shutil
import subprocess
import sys
import time
import typing

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
PROJECT_PATH = pathlib.Path(__file__).absolute().parent.parent / 'canonic.pro'

AWS_BUCKET_NAME = 'www.canonic.com'
AWS_CF_DISTRIBUTION_ID = 'E783CEX3P81S7'

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

os.chdir(OUTPUT_DIR)

wasm_file_name = 'canonic.{}.wasm'.format(BUILD_SUFFIX)
print("Compressing canonic.wasm via brotli to {}".format(wasm_file_name))
result = subprocess.run(['brotli', 'canonic.wasm', '-o', wasm_file_name])

js_file_name = 'canonic.{}.js'.format(BUILD_SUFFIX)
print("copying canonic.js to " + js_file_name)
shutil.copy(OUTPUT_DIR / 'canonic.js', OUTPUT_DIR / js_file_name)

os.chdir(OUTPUT_DIR)

print("Starting file uploads")
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
invalidation_id = create_invalidation(['/{}'.format(wasm_file_name),
                                       '/{}'.format(js_file_name),
                                       '/qtloader.js'], AWS_CF_DISTRIBUTION_ID)

print("invalidation_id:", invalidation_id)
