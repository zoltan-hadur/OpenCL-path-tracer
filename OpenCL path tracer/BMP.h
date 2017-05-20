#pragma once

#include <CL\cl.hpp>

class BMP {
private:
	BMP();	// "static class"
public:
	static void read(std::string file_path, std::vector<cl_uchar>& image, int& width, int& height);		// Reads uchar BGR image from file
	static void write(std::string file_path, std::vector<cl_uchar>& image, int& width, int& height);	// Writes uchar BGR image to file
};

void BMP::read(std::string file_path, std::vector<cl_uchar>& image, int& width, int& height) {
	unsigned char header[54];											// BMP header is 54 byte
	unsigned int image_size;											// width * height * 3

	FILE* file = fopen(file_path.c_str(), "rb");
	if (!file) {
		printf("Image could not be opened!\n");
		exit(EXIT_FAILURE);
	}
	if (fread(header, 1, 54, file) != 54) {								// Valid BMP contains at elast 54 byte
		printf("Not a correct BMP file!\n");
		exit(EXIT_FAILURE);
	}
	if (header[0] != 'B' || header[1] != 'M') {							// And starts with 'B', 'M'
		printf("Not a correct BMP file!\n");
		exit(EXIT_FAILURE);
	}

	width = *(int*)&(header[0x12]);										// Get the width of the image
	height = *(int*)&(header[0x16]);									// Get the height of the image
	image_size = *(int*)&(header[0x22]);								// Get the size of the image in bytes

	if (width > 2048 || height > 1024) {								// Bigger is not supported
		printf("Texture resolution is bigger than 2048x1024!\n");
		exit(EXIT_FAILURE);
	}
	if (image_size == 0) {
		image_size = width * height * 3;
	}

	image.clear();
	image.resize(image_size);
	fread(image.data(), 1, image_size, file);
	fclose(file);
}

void BMP::write(std::string file_path, std::vector<cl_uchar>& image, int& width, int& height) {
	int padding_bytes = (4 - ((width * 3) % 4)) % 4;			// The size of a horizontal line of the picture in bytes must be divisible by 4
	cl_uchar padding[3] = { 0, 0, 0 };							// So if it's not divisible, need to use padding
	int file_size = (width * 3 + padding_bytes) * height;		// Size of the image with padding
	unsigned char header[54];
	for (int i = 0; i < 54; ++i) {
		header[i] = 0;
	}
	header[0] = 'B';
	header[1] = 'M';
	*(int*)&(header[2]) = 54 + file_size;						// File size in bytes
	header[10] = 54;											// Offset where the pixel array (bitmap data) can be found
	header[14] = 40;											// Number of bytes in the DIB header(from this point)
	*(int*)&(header[18]) = width;								// Width of the bitmap in pixels
	*(int*)&(header[22]) = height;								// Height of the bitmap in pixels
	header[26] = 1;												// Number of color planes being used
	header[28] = 24;											// Number of bits per pixel
	header[34] = file_size;										// Size of the raw bitmap data (including padding)

	FILE* file;
	file = fopen(file_path.c_str(), "wb+");
	fwrite(header, 1, 54, file);								// Write the header to the file
	for (int i = 0; i < height; ++i) {
		fwrite(&image[width * 3 * i], 1, width * 3, file);		// Write a horizontal line of the image to the file
		fwrite(padding, 1, padding_bytes, file);				// Write the padding after the line to the file
	}
	fclose(file);
}