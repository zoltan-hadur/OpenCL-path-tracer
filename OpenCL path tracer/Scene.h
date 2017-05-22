#pragma once

#include <string>
#include <vector>
#include <utility>
#include "GLConsole.h"
#include "CLDevice.h"
#include "BMP.h"
#include "Camera.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Material.h"
#include "TextureInfo.h"

class Scene {
private:
	enum ToneMap {
		RAW, SRGB, REINHARD, FILMIC, ToneMap_COUNT
	};
	enum Mode {
		RAY_TRACING, PATH_TRACING, Mode_COUNT
	};

	GLuint canvas_id;		// CLDevice renders to this texture
	CLDevice device;
	Camera camera;
	std::vector<Sphere> spheres;
	std::vector<Triangle> triangles;
	std::vector<Material> materials;
	std::vector<std::vector<float>> textures;
	std::vector<std::vector<float>> bump_maps;
	std::vector<std::pair<cl_float, cl_float>> texture_resolutions;
	std::vector<TextureInfo> texture_infos;

	cl_int sample_id;
	cl_int max_depth;
	cl_int tone_map;
	cl_int mode;
public:
	Scene();
	void init(int width, int height);
	void add_sphere(Sphere obj);
	void add_triangle(Triangle obj);
	int add_material(Material mat);
	int add_texture(std::string file_path);
	int add_texture_info(cl_short index, cl_char flag);
	void commit();
	void next_tone_map();
	void next_mode();
	void render();
	void draw();
	void capture_picture();
};

Scene::Scene() {
	// Does nothing
}

void Scene::init(int width, int height) {
	GLConsole::add_function("commit()", std::bind(&Scene::commit, this));
	GLConsole::add_function("capture_picture()", std::bind(&Scene::capture_picture, this));

	device.init(width, height);
	canvas_id = device.get_canvas_id();
	camera = Camera(width, height);

	texture_infos.push_back(TextureInfo(0, 0, 0, 0));

	sample_id = 0;
	max_depth = 10;
	tone_map = RAW;
	mode = RAY_TRACING;
}

void Scene::add_sphere(Sphere obj) {
	spheres.push_back(obj);
}

void Scene::add_triangle(Triangle obj) {
	triangles.push_back(obj);
}

int Scene::add_material(Material mat) {
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

		glBindTexture(GL_TEXTURE_2D_ARRAY, canvas_id + 1);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, id, 2048, 1024, 1, GL_RGBA, GL_FLOAT, expanded_rgb.data());

		glBindTexture(GL_TEXTURE_2D_ARRAY, canvas_id + 2);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, id, 2048, 1024, 1, GL_RGBA, GL_FLOAT, expanded_bump_map.data());
	} else {
		std::cout << "Maximum number of textures reached!" << std::endl;
		return -1;
	}
}

int Scene::add_texture_info(cl_short index, cl_char flag) {
	texture_infos.push_back(TextureInfo(texture_resolutions[index].first, texture_resolutions[index].second, index, flag));
	return texture_infos.size() - 1;
}

void Scene::commit() {
	device.commit(spheres, triangles, materials, texture_infos);
}

void Scene::next_tone_map() {
	tone_map = (tone_map + 1) % ToneMap_COUNT;
}

void Scene::next_mode() {
	mode = (mode + 1) % Mode_COUNT;
}

void Scene::render() {
	device.generate_primary_rays(camera);
	device.trace_rays(sample_id, max_depth, mode);
	device.draw_screen(tone_map);
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

void Scene::capture_picture() {
	static int id = 0;
	std::string file_name = "image_" + std::to_string(id) + ".bmp";
	int width, height;
	std::vector<cl_uchar> image;
	glBindTexture(GL_TEXTURE_2D, canvas_id);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	image.resize(width * height * 3);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data());

	CreateDirectoryA("rendered", NULL);
	BMP::write("rendered/" + file_name, image, width, height);

	id++;
	GLConsole::cout << file_name + " created\n";
}