#pragma once

#include <string>
#include <vector>
#include <utility>
#include <atomic>
#include <mutex>
#include "GLConsole.h"
#include "CLDevice.h"
#include "BMP.h"
#include "Camera.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Material.h"
#include "TextureInfo.h"

extern HDC current_dc;
extern HGLRC current_context;

class Scene {
private:
	enum ToneMap {
		RAW, SRGB, REINHARD, FILMIC, ToneMap_COUNT
	};
	enum Mode {
		RAY_TRACING, PATH_TRACING, EXPLICIT_LIGHT_SAMPLING, Mode_COUNT
	};

	GLuint canvas_id;		// CLDevice renders to this texture
	CLDevice device;
	Camera camera;
	std::vector<Sphere*> spheres;
	std::vector<Triangle*> triangles;
	std::vector<Material*> materials;
	std::vector<std::vector<float>> textures;
	std::vector<std::vector<float>> bump_maps;
	std::vector<std::pair<cl_float, cl_float>> texture_resolutions;
	std::vector<TextureInfo> texture_infos;

	cl_uint sample_id;
	cl_uint max_depth;
	cl_uint tone_map;
	cl_uint mode;
	cl_uint filter;
	std::atomic_bool detach;
	std::mutex mtx;
public:
	Scene();
	void init(int width, int height);
	void add_sphere(Sphere* obj);
	void add_triangle(Triangle* obj);
	int add_material(Material* mat);
	int add_texture(std::string file_path);
	int add_texture_info(cl_uint index, cl_uint flag);
	Camera& get_camera();
	void commit();
	void next_tone_map();
	void next_mode();
	void next_filter();
	void render();
	void draw();
	void capture_screen();
};

Scene::Scene() {
	// Does nothing
}

void Scene::init(int width, int height) {
	sample_id = 0;
	max_depth = 7;
	tone_map = REINHARD;
	mode = RAY_TRACING;
	filter = 0;
	detach = false;

	GLConsole::add_function("commit()", std::bind(&Scene::commit, this));
	GLConsole::add_function("capture_screen()", std::bind(&Scene::capture_screen, this));

	device.init(width, height);
	canvas_id = device.get_canvas_id();
	camera = Camera(width, height);

	texture_infos.push_back(TextureInfo(0, 0, 0, 0));
}

void Scene::add_sphere(Sphere* obj) {
	spheres.push_back(obj);
}

void Scene::add_triangle(Triangle* obj) {
	triangles.push_back(obj);
}

int Scene::add_material(Material* mat) {
	materials.push_back(mat);
	return materials.size() - 1;
}

int Scene::add_texture(std::string file_path) {
	if (texture_resolutions.size() < device.get_max_textures()) {
		int id = texture_resolutions.size();
		int width, height;
		std::vector<cl_uchar> image;
		BMP::read(file_path, image, width, height);

		texture_resolutions.push_back({ width / 2048.0f, height / 1024.0f });

		std::vector<cl_float> rgb = device.bgr_to_rgb(image, width, height);
		std::vector<cl_float> grayscale = device.rgb_to_grayscale(rgb, width, height);
		std::vector<cl_float> bump_map = device.derivate_image(grayscale, width, height);
		std::vector<cl_float> expanded_rgb = device.expand_image(rgb, width, height);
		std::vector<cl_float> expanded_bump_map = device.expand_image(bump_map, width, height);

		device.upload_texture(expanded_rgb, canvas_id + 1, id);
		device.upload_texture(expanded_bump_map, canvas_id + 2, id);

		return id;
	} else {
		std::cout << "Maximum number of textures reached!" << std::endl;
		return -1;
	}
}

int Scene::add_texture_info(cl_uint index, cl_uint flag) {
	texture_infos.push_back(TextureInfo(texture_resolutions[index].first, texture_resolutions[index].second, index, flag));
	return texture_infos.size() - 1;
}

Camera& Scene::get_camera() {
	sample_id = 0;
	return camera;
}

void Scene::commit() {
	sample_id = 0;

	std::vector<Sphere> spheres_obj;
	for (int i = 0; i < spheres.size(); ++i) {
		spheres_obj.push_back(*spheres[i]);
	}
	std::vector<Triangle> triangles_obj;
	for (int i = 0; i < triangles.size(); ++i) {
		triangles_obj.push_back(*triangles[i]);
	}
	std::vector<Material> materials_obj;
	for (int i = 0; i < materials.size(); ++i) {
		materials_obj.push_back(*materials[i]);
	}
	mtx.lock();
	device.commit(spheres_obj, triangles_obj, materials_obj, texture_infos);
	mtx.unlock();
}

void Scene::next_tone_map() {
	tone_map = (tone_map + 1) % ToneMap_COUNT;
}

void Scene::next_mode() {
	sample_id = 0;
	mode = (mode + 1) % Mode_COUNT;
}

void Scene::next_filter() {
	filter = (filter + 1) % 3;
}

void Scene::render() {
	if (detach) {
		wglMakeCurrent(NULL, NULL);
	}
	while (!wglMakeCurrent(current_dc, current_context));
	mtx.lock();
	device.generate_primary_rays(camera);
	device.trace_rays(sample_id, max_depth, mode);
	device.draw_screen(tone_map, filter);
	mtx.unlock();
	sample_id++;
}

void Scene::draw() {
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, canvas_id);
	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 1); glVertex2f(-1.0f, 1.0f);
	glTexCoord2i(1, 1); glVertex2f(1.0f, 1.0f);
	glTexCoord2i(1, 0); glVertex2f(1.0f, -1.0f);
	glTexCoord2i(0, 0); glVertex2f(-1.0f, -1.0f);
	glEnd();
}

void Scene::capture_screen() {
	static int id = 0;
	std::string file_name = "image_" + std::to_string(id) + ".bmp";
	int width, height;
	std::vector<cl_uchar> image;

	detach = true;
	while (!wglMakeCurrent(current_dc, current_context));
	glBindTexture(GL_TEXTURE_2D, canvas_id);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	image.resize(width * height * 3);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data());
	wglMakeCurrent(NULL, NULL);
	detach = false;
	

	CreateDirectoryA("rendered", NULL);
	BMP::write("rendered/" + file_name, image, width, height);

	id++;
	GLConsole::cout << file_name + " created\n";
}