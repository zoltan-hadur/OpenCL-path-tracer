#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <GL\glew.h>
#include <GL\glext.h>
#include <GL\freeglut.h>
#include <CL\opencl.hpp>
#include <random>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Ray.h"
#include "Camera.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Material.h"
#include "TextureInfo.h"
#include "Color.h"

class CLDevice {
private:
	PFNGLTEXIMAGE3DPROC glTexImage3D;			// Function that needs to be loaded dynamically
	PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;		// Function that needs to be loaded dynamically

	int width, height;							// Width and height of the rendering canvas
	int max_textures;
	GLuint canvas_id;

	cl::Platform platform;
	cl::Device device;
	cl::Context context;
	cl::Program program;
	cl::CommandQueue queue;						// Command queue
	cl::ImageGL canvas;							// The device renders to this canvas
	cl::Buffer buffer_radiances;				// "colors"
	cl::Buffer buffer_rays;
	cl::Buffer buffer_randoms;

	cl::Buffer buffer_spheres;
	cl_int size_spheres;
	cl::Buffer buffer_triangles;
	cl_int size_triangles;
	cl::Buffer buffer_materials;

	cl::Buffer buffer_texture_infos;
	cl::ImageGL buffer_textures;
	cl::ImageGL buffer_bump_maps;
public:
	CLDevice();																						// Does nothing
	void init(int width, int height);																// Initializing the device. Should only get called once, should only have one instance and must be called after glutInit was called
	void upload_texture(std::vector<float>& image, int id, int index);
	void commit(std::vector<Sphere>& spheres, std::vector<Triangle>& triangles, std::vector<Material>& materials, std::vector<TextureInfo>& texture_infos);

	void generate_primary_rays(Camera camera);
	void trace_rays(cl_uint sample_id, cl_uint max_depth, cl_uint mode);
	void draw_screen(cl_uint tone_map, cl_uint filter);

	std::vector<cl_float> bgr_to_rgb(std::vector<cl_uchar> const& image, int width, int height);			// Convert uchar BGR image to float RGBA image
	std::vector<cl_float> rgb_to_grayscale(std::vector<cl_float>& image, int width, int height);	// Convert float RGBA image to float grayscale image
	std::vector<cl_float> derivate_image(std::vector<cl_float>& image, int width, int height);		// Derivate float grayscale image, returns float RGBA image
	std::vector<cl_float> expand_image(std::vector<cl_float>& image, int width, int height);		// Expands the image to 2048x1024 like GL_REPEAT

	int get_max_textures();																			// Returns the max number of textures
	GLuint get_canvas_id();																			// Return the canvas texture's id
	friend std::ostream& operator<<(std::ostream& os, CLDevice& device);
	friend std::istream& operator>>(std::istream& is, CLDevice& device);
};

CLDevice::CLDevice() {
	// Does nothing
}

void CLDevice::init(int width, int height) {
	//std::cout << "Initializing OpenCL device..." << std::endl;
	glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");								// Load function for allocate 3D texture
	glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)wglGetProcAddress("glTexSubImage3D");						// Load function for upload 3D texture

	this->width = width;
	this->height = height;

	if (cl::Platform::get(&platform) != CL_SUCCESS) {													// Try to get the first avaliable platform (driver)
		std::cout << "Error finding OpenCL platform (driver)!" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

	std::vector<cl::Device> devices;
	if (platform.getDevices(CL_DEVICE_TYPE_GPU, &devices) != CL_SUCCESS) {								// Try to get the first avaliable OpenCL capable GPU
		std::cout << "Error finding OpenCL capable GPU!" << std::endl;
		exit(EXIT_FAILURE);
	}
	device = devices[0];
	std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

	cl_int error_code = CL_SUCCESS;
	cl_context_properties properties[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),								// The current OpenGL context
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),										// The device context that is associated with the current OpenGL context
		CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(),										// The id of the used platform
		0																								// Terminating zero because we want OpenGL-OpenCL interoperability
	};
	context = cl::Context(device, properties, NULL, NULL, &error_code);									// Creating OpenCL context with the above properties
	if (error_code != CL_SUCCESS) {
		std::cout << "Error in creating OpenCL context" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::ifstream stream("kernel.cl");																	// Read the OpenCL kernel code
	std::stringstream str_stream;
	str_stream << stream.rdbuf();
	std::string kernel_code = str_stream.str();
	cl::Program::Sources sources;
	sources.push_back({ kernel_code.c_str(), kernel_code.length() });

	program = cl::Program(context, sources);															// Build the OpenCL program
	if (program.build({ device }) != CL_SUCCESS) {
		std::cout << "Error while building kernel code: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
		exit(EXIT_FAILURE);
	}

	queue = cl::CommandQueue(context, device);															// The command queue to execute kernel functions on the OpenCL device

	buffer_radiances = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Color) * width * height);			// Allocate memory on device
	buffer_rays = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Ray) * width * height);
	buffer_randoms = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_uint) * width * height);

	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	//std::minstd_rand rng(rd());
	std::uniform_int_distribution<int> uni(std::minstd_rand::min(), std::minstd_rand::max()); // guaranteed unbiased
	std::vector<cl_uint> seeds = std::vector<cl_uint>(width * height);
	for (cl_uint i = 0; i < seeds.size(); ++i) {
		//seeds[i] = i + 1;
		seeds[i] = uni(rng);
		//seeds[i] = 1;
	}
	queue.enqueueWriteBuffer(buffer_randoms, CL_TRUE, 0, sizeof(cl_uint) * width * height, seeds.data());// Upload seeds for random number generating to the device

	// Create canvas wich will be using while rendering
	GLuint texture;
	glGenTextures(1, &texture);
	canvas_id = texture;
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);	// Allocate memory on device for canvas
	canvas = cl::ImageGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texture);						// Create the OpenCL image from the OpenGL texture

  // Create FBO and attach texture to it
  GLuint fboId = 0;
  glGenFramebuffers(1, &fboId);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, canvas_id, 0);


	max_textures = device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() / 1024 / 1024 / 2 / 20;
	// Textures
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 2048, 1024, max_textures, 0, GL_RGBA, GL_FLOAT, NULL);

	// Bump maps
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 2048, 1024, max_textures, 0, GL_RGBA, GL_FLOAT, NULL);
}

void CLDevice::upload_texture(std::vector<float>& image, int id, int index) {
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, 2048, 1024, 1, GL_RGBA, GL_FLOAT, image.data());
}

void CLDevice::commit(std::vector<Sphere>& spheres, std::vector<Triangle>& triangles, std::vector<Material>& materials, std::vector<TextureInfo>& texture_infos) {
	size_spheres = spheres.size();
	size_triangles = triangles.size();

	buffer_spheres = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(Sphere)*spheres.size());
	queue.enqueueWriteBuffer(buffer_spheres, CL_TRUE, 0, sizeof(Sphere)*spheres.size(), spheres.data());

	buffer_triangles = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(Triangle)*triangles.size());
	queue.enqueueWriteBuffer(buffer_triangles, CL_TRUE, 0, sizeof(Triangle)*triangles.size(), triangles.data());

	buffer_materials = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(Material)*materials.size());
	queue.enqueueWriteBuffer(buffer_materials, CL_TRUE, 0, sizeof(Material)*materials.size(), materials.data());

	buffer_texture_infos = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(TextureInfo)*texture_infos.size());
	queue.enqueueWriteBuffer(buffer_texture_infos, CL_TRUE, 0, sizeof(TextureInfo)*texture_infos.size(), texture_infos.data());

	buffer_textures = cl::ImageGL(context, CL_MEM_READ_ONLY, GL_TEXTURE_2D_ARRAY, 0, canvas_id + 1);
	buffer_bump_maps = cl::ImageGL(context, CL_MEM_READ_ONLY, GL_TEXTURE_2D_ARRAY, 0, canvas_id + 2);
}



void CLDevice::generate_primary_rays(Camera camera) {
	cl::Kernel kernel_generate_primary_rays = cl::Kernel(program, "generate_primary_rays");
	kernel_generate_primary_rays.setArg(0, buffer_rays);
	kernel_generate_primary_rays.setArg(1, buffer_randoms);
	kernel_generate_primary_rays.setArg(2, camera);

	queue.enqueueNDRangeKernel(kernel_generate_primary_rays, cl::NullRange, cl::NDRange(width, height), cl::NullRange);
}

void CLDevice::trace_rays(cl_uint sample_id, cl_uint max_depth, cl_uint mode) {
	cl::Kernel kernel_trace_rays = cl::Kernel(program, "trace_rays");
	kernel_trace_rays.setArg(0, buffer_textures);
	kernel_trace_rays.setArg(1, buffer_bump_maps);
	kernel_trace_rays.setArg(2, buffer_texture_infos);
	kernel_trace_rays.setArg(3, buffer_spheres);
	kernel_trace_rays.setArg(4, size_spheres);
	kernel_trace_rays.setArg(5, buffer_triangles);
	kernel_trace_rays.setArg(6, size_triangles);
	kernel_trace_rays.setArg(7, buffer_materials);
	kernel_trace_rays.setArg(8, buffer_rays);
	kernel_trace_rays.setArg(9, buffer_randoms);
	kernel_trace_rays.setArg(10, buffer_radiances);
	kernel_trace_rays.setArg(11, sample_id);
	kernel_trace_rays.setArg(12, max_depth);
	kernel_trace_rays.setArg(13, mode);

	queue.enqueueNDRangeKernel(kernel_trace_rays, cl::NullRange, cl::NDRange(width, height), cl::NullRange);
}

void CLDevice::draw_screen(cl_uint tone_map, cl_uint filter) {
	cl::Kernel kernel_draw_screen = cl::Kernel(program, "draw_screen");
	kernel_draw_screen.setArg(0, canvas);
	kernel_draw_screen.setArg(1, buffer_radiances);
	kernel_draw_screen.setArg(2, tone_map);
	kernel_draw_screen.setArg(3, filter);

	queue.enqueueNDRangeKernel(kernel_draw_screen, cl::NullRange, cl::NDRange(width, height), cl::NullRange);
	queue.finish();
}



std::vector<cl_float> CLDevice::bgr_to_rgb(std::vector<cl_uchar> const& image, int width, int height) {
	cl::Buffer image_in = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar) * width * height * 3);
	cl::Buffer image_out = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * width * height * 4);
	queue.enqueueWriteBuffer(image_in, CL_TRUE, 0, width * height * 3, image.data());

	cl::Kernel kernel_bgr_to_rgb = cl::Kernel(program, "bgr_to_rgb");
	kernel_bgr_to_rgb.setArg(0, image_in);
	kernel_bgr_to_rgb.setArg(1, image_out);
	queue.enqueueNDRangeKernel(kernel_bgr_to_rgb, cl::NullRange, cl::NDRange(width * height));
	queue.finish();

	cl_float* tmp = new cl_float[width * height * 4];
	queue.enqueueReadBuffer(image_out, CL_TRUE, 0, sizeof(cl_float) * width * height * 4, tmp);
	std::vector<cl_float> ret = std::vector<cl_float>(tmp, tmp + width * height * 4);
	delete tmp;
	return ret;
}

std::vector<cl_float> CLDevice::rgb_to_grayscale(std::vector<cl_float>& image, int width, int height) {
	cl::Buffer image_in = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * width * height * 4);
	cl::Buffer image_out = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * width * height * 1);
	queue.enqueueWriteBuffer(image_in, CL_TRUE, 0, width * height * 4, image.data());

	cl::Kernel kernel_rgb_to_grayscale = cl::Kernel(program, "rgb_to_grayscale");
	kernel_rgb_to_grayscale.setArg(0, image_in);
	kernel_rgb_to_grayscale.setArg(1, image_out);
	queue.enqueueNDRangeKernel(kernel_rgb_to_grayscale, cl::NullRange, cl::NDRange(width * height));
	queue.finish();

	cl_float* tmp = new cl_float[width * height * 1];
	queue.enqueueReadBuffer(image_out, CL_TRUE, 0, sizeof(cl_float) * width * height * 1, tmp);
	std::vector<cl_float> ret = std::vector<cl_float>(tmp, tmp + width * height * 1);
	delete tmp;
	return ret;
}

std::vector<cl_float> CLDevice::derivate_image(std::vector<cl_float>& image, int width, int height) {
	cl::Buffer image_in = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * width * height * 1);
	cl::Buffer image_out = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * width * height * 4);
	queue.enqueueWriteBuffer(image_in, CL_TRUE, 0, width * height * 1, image.data());

	cl::Kernel kernel_derivate_image = cl::Kernel(program, "derivate_image");
	kernel_derivate_image.setArg(0, image_in);
	kernel_derivate_image.setArg(1, image_out);
	kernel_derivate_image.setArg(2, width);
	kernel_derivate_image.setArg(3, height);
	queue.enqueueNDRangeKernel(kernel_derivate_image, cl::NullRange, cl::NDRange(width * height));
	queue.finish();

	cl_float* tmp = new cl_float[width * height * 4];
	queue.enqueueReadBuffer(image_out, CL_TRUE, 0, sizeof(cl_float) * width * height * 4, tmp);
	std::vector<cl_float> ret = std::vector<cl_float>(tmp, tmp + width * height * 4);
	delete tmp;
	return ret;
}

std::vector<cl_float> CLDevice::expand_image(std::vector<cl_float>& image, int width, int height) {
	cl::Buffer image_in = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * width * height * 4);
	cl::Buffer image_out = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_float) * 2048 * 1024 * 4);
	queue.enqueueWriteBuffer(image_in, CL_TRUE, 0, sizeof(cl_float)*width*height * 4, image.data());

	cl::Kernel kernel_expand_image = cl::Kernel(program, "expand_image");
	kernel_expand_image.setArg(0, image_in);
	kernel_expand_image.setArg(1, image_out);
	kernel_expand_image.setArg(2, width);
	kernel_expand_image.setArg(3, height);
	queue.enqueueNDRangeKernel(kernel_expand_image, cl::NullRange, cl::NDRange(2048 * 1024));
	queue.finish();

	cl_float* tmp = new cl_float[2048 * 1024 * 4];
	queue.enqueueReadBuffer(image_out, CL_TRUE, 0, sizeof(cl_float) * 2048 * 1024 * 4, tmp);
	std::vector<cl_float> ret = std::vector<cl_float>(tmp, tmp + 2048 * 1024 * 4);
	delete tmp;
	return ret;
}

int CLDevice::get_max_textures() {
	return max_textures;
}

GLuint CLDevice::get_canvas_id() {
	return canvas_id;
}

std::ostream & operator<<(std::ostream& os, CLDevice& device) {
	os << "Information about the platform:\n";
	os << "\tCL_PLATFORM_NAME: " << device.platform.getInfo<CL_PLATFORM_NAME>() << "\n";
	os << "\tCL_PLATFORM_PROFILE: " << device.platform.getInfo<CL_PLATFORM_PROFILE>() << "\n";
	os << "\tCL_PLATFORM_VENDOR: " << device.platform.getInfo<CL_PLATFORM_VENDOR>() << "\n";
	os << "\tCL_PLATFORM_VERSION: " << device.platform.getInfo<CL_PLATFORM_VERSION>() << "\n";
	os << "\tCL_PLATFORM_EXTENSIONS: " << device.platform.getInfo<CL_PLATFORM_EXTENSIONS>() << "\n";

	os << "Information about the device:" << "\n";
	os << "\tCL_DEVICE_NAME: " << device.device.getInfo<CL_DEVICE_NAME>() << "\n";
	os << "\tCL_DEVICE_PROFILE: " << device.device.getInfo<CL_DEVICE_PROFILE>() << "\n";
	os << "\tCL_DEVICE_VENDOR: " << device.device.getInfo<CL_DEVICE_VENDOR>() << "\n";
	os << "\tCL_DEVICE_VERSION: " << device.device.getInfo<CL_DEVICE_VERSION>() << "\n";
	os << "\tCL_DRIVER_VERSION: " << device.device.getInfo<CL_DRIVER_VERSION>() << "\n";
	os << "\tCL_DEVICE_OPENCL_C_VERSION: " << device.device.getInfo<CL_DEVICE_OPENCL_C_VERSION>() << "\n";
	os << "\tCL_DEVICE_MAX_COMPUTE_UNITS: " << device.device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << "\n";
	os << "\tCL_DEVICE_EXTENSIONS: " << device.device.getInfo<CL_DEVICE_EXTENSIONS>() << "\n";

	os << "Resolution: " << device.width << "x" << device.height;
	return os;
}

std::istream & operator>>(std::istream& is, CLDevice& device) {
	return is;
}