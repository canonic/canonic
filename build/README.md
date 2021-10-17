# Build script
depends on the aws sdk https://github.com/boto/boto3

Install into ./lib/
python3.8 -m pip install boto3 -t .

## Dev Certificate
Generate a self-signed key and certificate for testing using:
```
openssl req  -nodes -new -x509  -keyout dev_server.key -out dev_server.cert
```

## Dev server
To test a `--no-deploy` build use the `dev_server.py` script. It  will pass through any requests for files not found in 
the build to production.

run with:
```
sudo python3.8 dev_server.py "/mnt/s/canonic/build-canonic-wasm/release/"
```

sudo required as it requires permission to bind to port 443 Optionally pass the directory is should server as an 
argument, otherwise it will use the current directory.