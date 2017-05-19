#pragma once

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS

#include <GL\glew.h>
#include <GL\glext.h>
#include <GL\freeglut.h>
#include <CL\cl.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Ray.h"

class OpenCL_Device {
private:
	PFNGLTEXIMAGE3DPROC glTexImage3D;			// Function that needs to be loaded dynamically
	PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;		// Function that needs to be loaded dynamically

	int screen_width, screen_height;			// Width and height of the rendering canvas
	int uploaded_textures, max_textures;

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
	cl::Buffer buffer_triangles;
	cl::Buffer buffer_toruses;
	cl::Buffer buffer_materials;

	cl::Buffer buffer_texture_nfos;
	cl::ImageGL buffer_textures;
	cl::ImageGL buffer_bump_maps;
public:
	OpenCL_Device();																				// Does nothing
	void init(int width, int height);																// Initializing the device. Should only get called once, should only have one instance and must be called after glutInit was called
	std::vector<cl_float> bgr_to_rgb(std::vector<cl_uchar>& image, int width, int height);			// Convert uchar BGR image to float RGBA image
	std::vector<cl_float> rgb_to_grayscale(std::vector<cl_float>& image, int width, int height);	// Convert float RGBA image to float grayscale image
	std::vector<cl_float> derivate_image(std::vector<cl_float>& image, int width, int height);		// Derivate float grayscale image, returns float RGBA image
	std::vector<cl_float> expand_image(std::vector<cl_float>& image, int width, int height);		// Expands the image to 2048x1024 like GL_REPEAT
	friend std::ostream& operator<<(std::ostream& os, OpenCL_Device& device);
	friend std::istream& operator>>(std::istream& is, OpenCL_Device& device);
};

OpenCL_Device::OpenCL_Device() {
	// Does nothing
}

void OpenCL_Device::init(int width, int height) {
	//std::cout << "Initializing OpenCL device..." << std::endl;
	glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");								// Load function for allocate 3D texture
	glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)wglGetProcAddress("glTexSubImage3D");						// Load function for upload 3D texture

	screen_width = width;
	screen_height = height;

	if (cl::Platform::get(&platform) != CL_SUCCESS) {													// Try to get the first avaliable platform (driver)
		std::cout << "Error finding OpenCL platform (driver)!" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Using platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

	std::vector<cl::Device> devices;
	if (platform.getDevices(CL_DEVICE_TYPE_GPU, &devices) != CL_SUCCESS) {								// Try to get the first avaliable OpenCL capable GPU
		std::cout << "Error finding OpenCL capable GPU!" << std::endl;
		if (platform.getDevices(CL_DEVICE_TYPE_CPU, &devices) != CL_SUCCESS) {							// Try to get the first avaliable OpenCL capable CPU if no GPU was found
			std::cout << "Error finding OpenCL capable CPU!" << std::endl;
			exit(EXIT_FAILURE);
		}
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

	buffer_radiances = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float3) * width * height);			// Allocate memory on device
	buffer_rays = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(Ray) * width * height);
	buffer_randoms = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_int) * width * height);

	std::vector<cl_int> seeds = std::vector<cl_int>(width * height);
	for (int i = 0; i < seeds.size(); ++i) {
		seeds[i] = i + 1;
	}
	queue.enqueueWriteBuffer(buffer_randoms, CL_TRUE, 0, sizeof(cl_int) * width * height, seeds.data());// Upload seeds for random number generating to the device

	// Create canvas wich will be using while rendering
	glEnable(GL_TEXTURE_2D);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT, NULL);	// Allocate memory on device for canvas
	canvas = cl::ImageGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texture);						// Create the OpenCL image from the OpenGL texture
}

std::vector<cl_float> OpenCL_Device::bgr_to_rgb(std::vector<cl_uchar>& image, int width, int height) {
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

std::vector<cl_float> OpenCL_Device::rgb_to_grayscale(std::vector<cl_float>& image, int width, int height) {
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

std::vector<cl_float> OpenCL_Device::derivate_image(std::vector<cl_float>& image, int width, int height) {
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

std::vector<cl_float> OpenCL_Device::expand_image(std::vector<cl_float>& image, int width, int height) {
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

std::ostream & operator<<(std::ostream& os, OpenCL_Device& device) {
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

	os << "Resolution: " << device.screen_width << "x" << device.screen_height;
	return os;
}

std::istream & operator>>(std::istream& is, OpenCL_Device& device) {
	return is;
}