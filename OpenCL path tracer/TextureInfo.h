#pragma once

#include <CL\opencl.hpp>

#pragma pack(1)
__declspec(align(4))
class TextureInfo {
private:
	cl_float width;		// Width of the texture
	cl_float height;	// Height of the texture
	cl_uint index;		// Index to the texture
	cl_uint flag;		// 0-nothing, 1-texture, 2-bump map
public:
	TextureInfo(cl_float width, cl_float height, cl_uint index, cl_uint flag);
};
#pragma pack()

TextureInfo::TextureInfo(cl_float width, cl_float height, cl_uint index, cl_uint flag) {
	this->width = width;
	this->height = height;
	this->index = index;
	this->flag = flag;
}