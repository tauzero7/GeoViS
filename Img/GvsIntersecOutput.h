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

    int  dataSize();

    bool getData(int i, int j, gvsData* dat);

    void setData(int i, int j, gvsData &dat);

    bool write(const char* filename);

private:
    // list of gvsData for every pixel

    int width, height;
    gvsData* m_data;
};

#endif // GVS_INTERSEC_OUTPUT_H
