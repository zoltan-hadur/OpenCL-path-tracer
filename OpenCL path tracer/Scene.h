#pragma once

#include <string>
#include <vector>
#include <utility>
#include "GLConsole.h"
#include "CLDevice.h"
#include "Bitmap.h"
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
		RAY_TRACING, PATH_TRACING, EXPLICIT_LIGHT_SAMPLING, Mode_COUNT
	};

  int width;
  int height;
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
	void inc_depth();
	void dec_depth();
	void render();
	void draw();
	void capture_screen();
};

Scene::Scene() {
	// Does nothing
}

void Scene::init(int width, int height) {
  this->width = width;
  this->height = height;

	sample_id = 0;
	max_depth = 7;
	tone_map = REINHARD;
	mode = RAY_TRACING;
	filter = 0;

	GLConsole::add_function("app.commit(void)", std::bind(&Scene::commit, this));
	GLConsole::add_function("app.capture_screen(void)", std::bind(&Scene::capture_screen, this));
	GLConsole::add_function("app.camera.look_at(float, float, float)", std::bind(&Camera::look_at, &camera, std::placeholders::_1));
	GLConsole::add_function("app.camera.set_pos(float, float, float)", std::bind(&Camera::set_pos, &camera, std::placeholders::_1));

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
		auto bitmap = Bitmap::Read(file_path);

		texture_resolutions.push_back({ bitmap.Width() / 2048.0f, bitmap.Height() / 1024.0f });

		std::vector<cl_float> rgb = device.bgr_to_rgb(bitmap.Bytes(), bitmap.Width(), bitmap.Height());
		std::vector<cl_float> grayscale = device.rgb_to_grayscale(rgb, bitmap.Width(), bitmap.Height());
		std::vector<cl_float> bump_map = device.derivate_image(grayscale, bitmap.Width(), bitmap.Height());
		std::vector<cl_float> expanded_rgb = device.expand_image(rgb, bitmap.Width(), bitmap.Height());
		std::vector<cl_float> expanded_bump_map = device.expand_image(bump_map, bitmap.Width(), bitmap.Height());

		device.upload_texture(expanded_rgb, canvas_id + 1, id);
		device.upload_texture(expanded_bump_map, canvas_id + 2, id);

		return id;
	} else {
		throw "Maximum number of textures reached!";
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

	device.commit(spheres_obj, triangles_obj, materials_obj, texture_infos);
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

void Scene::inc_depth() {
	sample_id = 0;
	max_depth++;
}

void Scene::dec_depth() {
	sample_id = 0;
	if (max_depth > 0) {
		max_depth--;
	}
}

void Scene::render() {
	device.generate_primary_rays(camera);
	device.trace_rays(sample_id, max_depth, mode);
	device.draw_screen(tone_map, filter);
	sample_id++;
}

void Scene::draw() {
  int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
  int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
  int width;
  int height;
  int posX;
  int posY;
  if ((float)this->width / (float)this->height < (float)windowWidth / (float)windowHeight)
  {
    height = windowHeight;
    width = (float)windowHeight * (float)this->width / (float)this->height;
    posX = (windowWidth - width) / 2.0;
    posY = 0;
  }
  else
  {
    width = windowWidth;
    height = windowWidth / ((float)this->width / (float)this->height);
    posX = 0;
    posY = (windowHeight - height) / 2.0;
  }
  glBlitFramebuffer(0, 0, this->width, this->height, posX, posY, posX + width, posY + height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Scene::capture_screen() {
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
    Bitmap(width, height, std::move(image)).Write("rendered/" + file_name);

	id++;
	GLConsole::cout << file_name + " created\n";
}