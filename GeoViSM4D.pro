
TOP_DIR  = $$PWD
HOME     = $$(HOME)

#message($$HOME)

## -------------------------------------------------------------
##  Enter the path to your Gnu Scientific Library installation:
## -------------------------------------------------------------
GSL_DIR     = /usr/local/gsl/1.15
GSL_LIB_DIR = $$GSL_DIR/lib

## -------------------------------------------------------------
##  Enter the path to your libMotion4D installation:
## -------------------------------------------------------------
CONFIG += USE_M4D_SRC
M4D_DIR     = ../libMotion4D
M4D_SRC_DIR = $$M4D_DIR/src

## -------------------------------------------------------------
##  If you have tiff library, adapt the paths. Otherwise,
##  remove "USE_TIFF".
## -------------------------------------------------------------
CONFIG  += USE_TIFF
TIFF_DIR     = /usr
TIFF_LIB_DIR = $$TIFF_DIR/lib

## -------------------------------------------------------------
##  If you have png library, adapt the paths. Otherwise,
##  remove "USE_PNG".
## -------------------------------------------------------------
CONFIG  += USE_PNG
PNG_DIR     = /usr
PNG_LIB_DIR = $$PNG_DIR/lib


## =============================================================
##  You should not need to modify the lines below...

PROJECT_MAIN = geovis.cpp

CONFIG += $$QMAKE_HOST.arch
i686 {
    BITS = 32
} else {
    BITS = 64
}

CONFIG  += USE_PPM

DEFINES += #GVS_VERBOSE

CONFIG(debug, debug|release) {
    TARGET = gvsRender$$BITS"d"
    OBJECTS_DIR = $$TOP_DIR/compiled/serial/debug
}

CONFIG(release, debug|release) {
    TARGET = gvsRender$$BITS
    OBJECTS_DIR = $$TOP_DIR/compiled/serial/release
}

include( GeoViSM4D.pri )
