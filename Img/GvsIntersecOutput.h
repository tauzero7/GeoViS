#ifndef GVS_INTERSEC_OUTPUT_H
#define GVS_INTERSEC_OUTPUT_H

#include "GvsGlobalDefs.h"

class GvsIntersecOutput
{
public:
    GvsIntersecOutput();
    GvsIntersecOutput(int width, int height);

    void clear();

    void resize(int width, int height);

private:
    // list of gvsData for every pixel
};

#endif // GVS_INTERSEC_OUTPUT_H
