#pragma once

#include <CL\cl.hpp>

class TextureInfo {
private:
	cl_float width, height;
	cl_short index;			// Index to texture
	cl_char flag;			// 0-nothing, 1-texture, 2-bump map
public:
	TextureInfo(cl_float width, cl_float height, cl_short index, cl_char flag);
};

TextureInfo::TextureInfo(cl_float width, cl_float height, cl_short index, cl_char flag) {
	this->width = width;
	this->height = height;
	this->index = index;
	this->flag = flag;
}