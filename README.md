# Canonic

## Other QMl Web Browsers:
https://github.com/comcomist/Qdolphin
https://github.com/penk/SlateKit/tree/master/Shell
https://github.com/gustavosbarreto/qml-browser
https://github.com/jacob3141/methane

# How to compile qt for webassembly

mkdir qt
cd qt

## First build Qt for the host
Since Qt 6 cross compiling requires that you first install qt on your host system

mkdir build_host
cd build_host
../qt-everywhere-src-6.2.0-beta3/configure -opensource -nomake examples -prefix $PWD/qtbase
cmake --build . --parallel

Note: skip the --parallel if you run out of ram


git clone git://code.qt.io/qt/qt5.git
cd qt5
git checkout 5.15 (not 5.15.0 or 5.15.1 or 5.15.2)
perl init-repository --module-subset=default,-qtwebengine

cd ..
mkdir build
cd build

You can also grab the sources from https://download.qt.io/development_releases/qt/ if you do not want to use the git version

../qt-everywhere-src-6.2.0-beta3/configure -qt-host-path /mnt/s/qt/build_host/qtbase -xplatform wasm-emscripten -opensource -nomake examples -prefix $PWD/qtbase
cmake --build . -t qtbase -t qtdeclarative -t qt5compat -t qtquick3d -t qtimageformats -t qtshadertools -t qtwebsockets -t qtsvg -t qtmqtt

note the `module-qtmqtt` package did not work for me the second time so maybe leave it out
note: add `-debug` to the configure step for a debug build

# How to compile for webassembly
Add emscripten to the environment (2.0.15 worked for me (latest))  

cd /mnt/s/emscripten/emsdk/
source ./emsdk_env.sh

cd /mnt/s/canonic/
mkdir build-canonic-wasm
cd build-canonic-wasm
/mnt/s/qt/build/qtbase/bin/qmake /mnt/hgfs/s/canonic/canonic/canonic.pro "CONFIG-=debug" "CONFIG+=release" "CONFIG+=wasm"
make release

Watch out for browsers caching your .wasm files


if you encounter:
`wasm-ld: error: initial memory too small, 17787616 bytes needed`
Then add `-s INITIAL_MEMORY=33554432` to `LFLAGS` in the Makefile

## Debug build
add `--source-map-base http://localhost:8000/ -g4` if you want source maps (use debug build of qt for more info)

/mnt/s/qt/build-debug/qtbase/bin/qmake /mnt/hgfs/s/canonic/canonic/canonic.pro "CONFIG-=release" "CONFIG+=debug" "CONFIG+=wasm"


## Brotli compression
brotli canonic.wasm -o canonic.release.wasm

## Build script
To build and upload to aws

cd /mnt/hgfs/s/canonic/canonic/build
./build.py



python3.8 -m http.server

# How to pass environment variables:
var qtLoader = QtLoader({
    canvasElements : [canvas],
    environment : {"QT_QUICK_BACKEND" : "software"},
