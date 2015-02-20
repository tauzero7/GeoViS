
PROJECT_MAIN = geovis.cpp

CONFIG += $$system(uname -m)
x86_64 {
    BITS = 64
} else {
    BITS = 32
}

TOP_DIR  = $$PWD

HOME     = $$system(echo $HOME)
GSL_DIR  = $$system(echo $GSL_INSTALL_PATH)

M4D_DIR     = $$HOME/Projekte/libMotion4D
M4D_SRC_DIR = $$M4D_DIR/src
CONFIG += USE_M4D_SRC

CONFIG  += USE_TIFF # USE_PPM
TIFF_LIB_DIR = $$system(echo $TIFF_LIB_PATH)

DEFINES += #GVS_VERBOSE

CONFIG(debug, debug|release) {
    TARGET = gvsRender$$BITS"d"
    OBJECTS_DIR = $$TOP_DIR/compiled/hestia/serial/debug
}

CONFIG(release, debug|release) {
    TARGET = gvsRender$$BITS
    OBJECTS_DIR = $$TOP_DIR/compiled/hestia/serial/release
}

include( GeoViSM4D.pri )

LIBS += -L$$TIFF_LIB_DIR -Wl,-rpath $$TIFF_LIB_DIR 

