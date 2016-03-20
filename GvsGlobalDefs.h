#ifndef GVS_GLOBAL_DEFS_H
#define GVS_GLOBAL_DEFS_H

#include <m4dGlobalDefs.h>

#ifdef __GNUC__
#define UNUSED_ATTR  __attribute__((unused))
#endif

#ifdef _POSIX_SOURCE
#define M_PI    3.14159265358979323846
#define M_PI_2  1.57079632679489661923
#define M_PI_4  0.78539816339744830962
#define MPITWO  6.28318530717958647692
#define M_EUL   2.71828182845904523536
#define M_EINV  0.36787944117144232160
#endif // _POSIX_SOURCE


#ifndef DBL_MAX
#define DBL_MAX 1.844674407370955616e19
#endif

#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif


#define GVS_deg2rad  0.017453292519943295770
#define GVS_rad2deg  57.295779513082320875


#define GVS_SPEED_OF_LIGHT       2.99792458e8
#define GVS_GRAVITATIONAL_KONST  6.6742e-11
#define GVS_SOLAR_MASS           1.989e30
#define GVS_ELECTR_VACUUM_CONST  8.85418782e-12


#define GVS_ROUND(doubleval) (((doubleval) > 0.0) ? floor((doubleval)+0.5) : ceil((doubleval)-0.5))

#define GVS_TRUNC(doubleval) (((doubleval) > 0.0) ? floor((doubleval)) : ceil((doubleval)))

#define GVS_EPS    1.0e-10

#define GVS_MAX(a,b) (((a)>(b))?(a):(b))
#define GVS_MIN(a,b) (((a)<(b))?(a):(b))

#define GVS_MIN2(a,b)     GVS_MIN(a,b)
#define GVS_MAX2(a,b)     GVS_MAX(a,b)
#define GVS_MIN3(a,b,c)   ( GVS_MIN( a, GVS_MIN(b,c) ) )
#define GVS_MAX3(a,b,c)   ( GVS_MAX( a, GVS_MAX(b,c) ) )
#define GVS_MIN4(a,b,c,d) ( GVS_MIN( GVS_MIN(a,b), GVS_MIN(c,d) ) )
#define GVS_MAX4(a,b,c,d) ( GVS_MAX( GVS_MAX(a,b), GVS_MAX(c,d) ) )

#define GVS_DELTA(a,b) (((a)==(b))?(1):(0))

#define GVS_SIGN(a) ((a>0)?(1):(-1))


typedef unsigned long int   ulong;
typedef unsigned int        uint;
typedef unsigned short int  ushort;
typedef unsigned char       uchar;


enum GvsDataType {
    gvsDT_UNKNOWN = 0,
    gvsDT_INT,
    gvsDT_FLOAT,
    gvsDT_DOUBLE,
    gvsDT_VEC2,
    gvsDT_VEC3,
    gvsDT_VEC4,
    gvsDT_MAT2D,
    gvsDT_MAT3D,
    gvsDT_IVEC2,
    gvsDT_IVEC3,
    gvsDT_IVEC4,
    gvsDT_STRING,
    gvsDT_VOID
};

const std::string GvsDataTypeName[14] = {
    "gvsDT_UNKNOWN",
    "gvsDT_INT",
    "gvsDT_FLOAT",
    "gvsDT_DOUBLE",
    "gvsDT_VEC2",
    "gvsDT_VEC3",
    "gvsDT_VEC4",
    "gvsDT_MAT2D",
    "gvsDT_MAT3D",
    "gvsDT_IVEC2",
    "gvsDT_IVEC3",
    "gvsDT_IVEC4",
    "gvsDT_STRING",
    "gvsDT_VOID"
};


struct gvs_parameter {
    GvsDataType  type;
    void*        val;
};


enum GvsSetParamHint {
    gvsSetParamError = -1,
    gvsSetParamNone = 0,
    gvsSetParamAdjustTetrad
};


enum GvsCamFilter {
    gvsCamFilterRGB = 0,   // rgb image only (default)
    gvsCamFilterRGBpdz,    // rgb image + position-,direction-4-vectors + freqshift
    gvsCamFilterRGBjac,    // rgb image + position-,direction-4-vectors + freqshift + Jacobi
    gvsCamFilterRGBpt,     // rgb image + position-4-vector + texture
    gvsCamFilterIntersec   // save all intersections
};

const int GvsNumCamFilters = 5;
const std::string GvsCamFilterNames[GvsNumCamFilters] = {
    "FilterRGB",
    "FilterRGBpdz",
    "FilterRGBjac",
    "FilterRGBpt",
    "FilterIntersec"
};

enum GvsCamEye {
    gvsCamEyeStandard = 0,
    gvsCamEyeLeft,
    gvsCamEyeRight
};

const int GvsNumCamEyes = 3;
const std::string GvsCamEyeNames[GvsNumCamEyes] = {
    "Standard",
    "LeftEye",
    "RightEye"
};
const std::string GvsCamEyeFileExt[GvsNumCamEyes] = {
    ".",
    ".left.",
    ".right."
};

#define  NUM_PDZ_CHANNELS  10
#define  NUM_JAC_CHANNELS  15
#define  NUM_PT_CHANNELS    7

typedef struct gvsData_T {
    double    objID;       // object ID of intersection object
    double    pos[4];      // position of intersection points
    double    dir[4];      // direction of light ray at intersection point (future directed)
    double    freqshift;   // gravitational frequency shift
    double    jacobi[5];   // jacobi parameters
    double    uv[2];       // uv texture coordinates
    gvsData_T() {
        objID = 0.0;
        pos[0] = pos[1] = pos[2] = pos[3] = 0.0;
        dir[0] = dir[1] = dir[2] = dir[3] = 0.0;
        freqshift = 0.0;
        jacobi[0] = jacobi[1] = jacobi[2] = jacobi[3] = jacobi[4] = 0.0;
        uv[0] = uv[1] = 0.0;
    }
} gvsData;


#endif

