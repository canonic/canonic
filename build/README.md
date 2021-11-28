# Build script
The following README describes the Canonic WASM build scripts. For native builds just use Qt's standard build systems 
(i.e load  project into QtCreator and click build). 

## build.py
`build.py` is the main build script for building Canonic with wasm, 

### Dependencies
The following dependencies should be installed into a `./lib/` directory before running `build.py`.

1. `cd /path/to/canonic/build/`
2. `mkdir lib`
3. `cd lib`
4. `python3.8 -m pip install boto3 -t .` (about: https://github.com/boto/boto3)

### Usage:

Build and deploy a production release:
`./build`

Build a production release without deploying files to AWS:
`./build -no-deploy`

## Dev server

To test a Canonic WASM build without deploying follow the below steps:

### 1. Modify hosts file
Modify your systems hosts file so that https://www.canonic.com resolves to 127.0.0.1. To do that add the following line
to your hosts file:
```
127.0.0.1 www.canonic.com
```
#### Where is my hosts file?
Your hosts file is located in one of the following paths:

| OS      | hosts file path                         |
|---------|-----------------------------------------|
| Linux   | `C:/windows/system32/drivers/etc/hosts` |
| Windows | `etc/hosts`                             |
| MacOS   | `etc/hosts`                             |

### 2. Create a no-deploy build
Navigate to this build folder and run `./build.py -no-deploy` to generate a no deploy build. This will output all the 
required files for testing Canonic without uploading the files to AWS or running additional compression techniques on 
them.

### 3. Run dev_server.py

Run the `dev_server.py` script using the following format:
```
sudo python3.8 dev_server.py "/path/to/no-depoy/build"
```

>**sudo** required as it requires permission to bind to port 443. Second argument is an optional directory the server 
> should serve, otherwise it will use the current directory.

The `dev_server.py` script is a simple local http server written in Python that should be used for serving the no deploy
build created in step 2. It is a very simple passthrough server which resolves requests using the following rules:
- If the HTTP(S) request matches a local file path under the servers serve path then it returns that local file.
- If the HTTP(S) request does not match a local file then it returns the remote response (e.g. what 
  https://www.canonic.com/your/path would return). Note it bypasses the hosts rule created in step 1 for this remote
  passthrough request by using `dns.google` to externally resolve the IP address for `www.canonic.com`.

The above passthrough logic is required because there are static files hosted under https://www.canonic.com that are not
part of the Canonic build, so in order to fully test it locally these files still need to be accessible.

`dev_server.py` can also be useful if you do not want to create a full no deploy build but just have a few http 
endpoints that you want to override.

#### Known dev_server.py limitations
- It's single threaded, so it's pretty slow compared to an actual webserver

## Dev Certificate
`dev_server.py` requires both a `dev_server.cert` and `dev_server.key` file to exist in order to support https testing. 
A self signed test certificate and key are provided in version control for this purpose.

The following `openssl` command was used to generate both files:
```
openssl req  -nodes -new -x509  -keyout dev_server.key -out dev_server.cert
```

> These files should obviously only ever be used for local testing, real **.key** files should be kept private in the real world!