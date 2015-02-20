
PROJECT_MAIN = geovispar.cpp

CONFIG += $$system(uname -m)
x86_64 {
    BITS = 64
} else {
    BITS = 32
}

TOP_DIR  = $$PWD

QMAKE_CC   = mpicc
QMAKE_CXX  = mpic++
QMAKE_LINK = mpic++

HOME     = $$system(echo $HOME)

M4D_DIR     = $$HOME/Projekte/libMotion4D
M4D_SRC_DIR = $$M4D_DIR/src
CONFIG += USE_M4D_SRC

GSL_DIR      = $$system(echo $GSL_INSTALL_PATH)
TIFF_LIB_DIR = $$system(echo $TIFF_INSTALL_PATH)/lib

CONFIG  += USE_TIFF # USE_PPM
DEFINES += #GVS_VERBOSE

CONFIG(debug, debug|release) {
    TARGET = gvsRenderPar$$BITS"d"_hestia
    OBJECTS_DIR = $$TOP_DIR/compiled/hestia/parallel/debug
}

CONFIG(release, debug|release) {
    TARGET = gvsRenderPar$$BITS
    OBJECTS_DIR = $$TOP_DIR/compiled/hestia/parallel/release
}

include( GeoViSM4D.pri )

GMP_DIR = $$TOP_DIR/MpiUtils

HEADERS += $$GMP_DIR/GvsMpiDefs.h \
           $$GMP_DIR/MpiImage.h \
           $$GMP_DIR/MpiTaskManager.h

SOURCES += $$GMP_DIR/MpiImage.cpp \
           $$GMP_DIR/MpiTaskManager.cpp

INCLUDEPATH += $$MPI_DIR/include

LIBS += -Wl,-rpath $$MPI_DIR/lib


