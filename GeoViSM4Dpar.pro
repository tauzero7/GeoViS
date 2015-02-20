

TOP_DIR  = $$PWD
HOME     = $$(HOME)

#message($$HOME)

## -------------------------------------------------------------
##  Enter the path to your Gnu Scientific Library installation:
## -------------------------------------------------------------
GSL_DIR     = /usr/local/gsl/1.15
GSL_LIB_DIR = $$GSL_DIR/lib
LIBS += -Wl,-rpath $$GSL_LIB_DIR

## -------------------------------------------------------------
##  Enter the path to your libMotion4D installation:
## -------------------------------------------------------------
CONFIG += USE_M4D_SRC
M4D_DIR     = ../libMotion4D
M4D_SRC_DIR = $$M4D_DIR/src

## -------------------------------------------------------------
##  Enter the path to your MPI library installation:
## -------------------------------------------------------------
MPI_DIR     = /usr/local/openmpi/1.6.4
MPI_LIB_DIR = $$MPI_DIR/lib
LIBS += -Wl,-rpath $$MPI_LIB_DIR

## -------------------------------------------------------------
##  If you have tiff library, adapt the paths.
##  Otherwise, remove "USE_TIFF".
## -------------------------------------------------------------
CONFIG  += USE_TIFF
TIFF_DIR     = /usr
TIFF_LIB_DIR = $$TIFF_DIR/lib
#LIBS += -Wl,-rpath $$TIFF_LIB_DIR

## -------------------------------------------------------------
##  If you have png library, adapt the paths.
##  Otherwise, remove "USE_PNG".
## -------------------------------------------------------------
CONFIG  += USE_PNG
PNG_DIR     = /usr
PNG_LIB_DIR = $$PNG_DIR/lib
#LIBS += -Wl,-rpath $$PNG_LIB_DIR

## =============================================================
##  You should not need to modify the lines below...

PROJECT_MAIN = geovispar.cpp

CONFIG += $$QMAKE_HOST.arch
i686 {
    BITS = 32
} else {
    BITS = 64
}


QMAKE_CC   = $$MPI_DIR/bin/mpicc
QMAKE_CXX  = $$MPI_DIR/bin/mpic++
QMAKE_LINK = $$MPI_DIR/bin/mpic++

CONFIG  += USE_PPM
DEFINES += #GVS_VERBOSE

CONFIG(debug, debug|release) {
    TARGET = gvsRenderPar$$BITS"d"
    OBJECTS_DIR = $$TOP_DIR/compiled/parallel/debug
}

CONFIG(release, debug|release) {
    TARGET = gvsRenderPar$$BITS
    OBJECTS_DIR = $$TOP_DIR/compiled/parallel/release
}

LIBS += -L$$MPI_LIB_DIR -lmpi

include( GeoViSM4D.pri )

GMP_DIR = $$TOP_DIR/MpiUtils

HEADERS += $$GMP_DIR/GvsMpiDefs.h \
           $$GMP_DIR/MpiImage.h \
           $$GMP_DIR/MpiTaskManager.h

SOURCES += $$GMP_DIR/MpiImage.cpp \
           $$GMP_DIR/MpiTaskManager.cpp

INCLUDEPATH += $$MPI_DIR/include


