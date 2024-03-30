#include "TextureInfo.h"

TextureInfo::TextureInfo(cl_float width, cl_float height, cl_uint index, cl_uint flag)
{
    this->width = width;
    this->height = height;
    this->index = index;
    this->flag = flag;
}