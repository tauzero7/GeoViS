
#include "GvsIntersecOutput.h"

GvsIntersecOutput::GvsIntersecOutput() :
    m_data(NULL)
{
}


GvsIntersecOutput::GvsIntersecOutput(int width, int height) {
    m_data = NULL;
    resize(width, height);
}


void GvsIntersecOutput::clear() {
    if (m_data != NULL) {
        delete[] m_data;
        m_data = NULL;
    }
}


void GvsIntersecOutput::resize(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    clear();
    this->width = width;
    this->height = height;
    m_data = new gvsData[width * height];
    assert(m_data != NULL);
}

int GvsIntersecOutput::dataSize() {
    if (m_data == NULL) {
        return 0;
    }

    return this->width * this->height;
}


bool GvsIntersecOutput::getData(int i, int j, gvsData *dat) {
    if (m_data != NULL && i >= 0 && i < width && j >= 0 && j < height) {
        *dat = m_data[j * width + i];
        return true;
    }
    return false;
}


void GvsIntersecOutput::setData(int i, int j, gvsData &dat) {
    if (m_data != NULL && i >= 0 && i < width && j >= 0 && j < height) {
        m_data[j * width + i] = dat;
    }
}


bool GvsIntersecOutput::write(const char* filename) {
    fprintf(stderr, "GvsIntersecOutput() .. save file '%s'\n", filename);
    FILE* fptr = fopen(filename, "wb");
    if (fptr == NULL) {
        fprintf(stderr, "Cannot open file for output!\n");
        return false;
    }
    fwrite((void*)&width, sizeof(int), 1, fptr);
    fwrite((void*)&height, sizeof(int), 1, fptr);
    fwrite(m_data, sizeof(gvsData), width * height, fptr);
    fclose(fptr);
    return true;
}
