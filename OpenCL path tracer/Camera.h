#pragma once

#include <sstream>
#include "Float3.h"
#include "Ray.h"
#include "GLConsole.h"

class Camera {
private:
	Float3 pos;									// Position of the eye
	Float3 right;								// X axis of the camera's coordinate system
	Float3 up;									// Y axis of the camera's coordinate system
	Float3 forward;								// Z axis of the camera's coordinate system
	cl_float focus_distance;					// Focus distance
	cl_float fov;								// Horizontal field of vision in �
	cl_uint width;								// Width of the canvas
	cl_uint height;								// Height of the canvas
public:
	Camera(cl_uint width, cl_uint height);		// Initializes the camera.
	void rotate(float dpitch, float dyaw);		// Rotates the camera by the two angle in degrees.
	void translate_right(float v);				// Moves the camera along it's x axis.
	void translate_up(float v);					// Moves the camera along it's y axis.
	void translate_forward(float v);			// Moves the camera along it's z axis.
	void increase_fov(float dt);				// Increases the field of vision.
	void decrease_fov(float dt);				// Decreases the field of vision.
	void increase_focus_distance(float dt);		// Increases the focus distance.
	void decrease_focus_distance(float dt);		// Decreases the focus distance.
	void set_focus_distance(float d);			// Sets the focus distance.
	Ray get_ray(int x, int y);					// Generates a ray that originates from the camera and points toward to a point on the screen defined by x y.
	void look_at(std::vector<std::string> params);
	void set_pos(std::vector<std::string> params);
};

Camera::Camera(cl_uint width = 600, cl_uint height = 600) {
	focus_distance = 1000.0f;
	fov = 90.0f;
	this->width = width;
	this->height = height;

	float right_length = width / 2.0f;
	float up_length = height / 2.0f;
	float forward_length = right_length / tan(fov / 2.0f / 180.0f * 3.14159265358979323846f);

	pos = Float3(800.0f, 500.0f, -1500.0f);
	right = Float3(right_length, 0.0f, 0.0f);
	up = Float3(0.0f, up_length, 0.0f);
	forward = Float3(0.0f, 0.0f, forward_length);
}

void Camera::rotate(float dpitch, float dyaw) {
	Float3 y_axis = Float3(0, 1, 0);

	right = Float3::rodrigues(right, y_axis, dyaw);
	up = Float3::rodrigues(up, y_axis, dyaw);
	forward = Float3::rodrigues(forward, y_axis, dyaw);

	up = Float3::rodrigues(up, right.normalize(), dpitch);
	forward = Float3::rodrigues(forward, right.normalize(), dpitch);
}

void Camera::translate_right(float v) {
	pos = pos + right.normalize() * v;
}

void Camera::translate_up(float v) {
	pos = pos + up.normalize() * v;
}

void Camera::translate_forward(float v) {
	pos = pos + forward.normalize() * v;
}

void Camera::increase_fov(float dt) {
	float fov = atan(right.length() / forward.length()) * 2 * 180 / 3.14159265358979323846f;
	if (fov + dt < 175) {
		fov = fov + dt;
		forward = forward.normalize() * (right.length() / tan(fov / 2 / 180 * 3.14159265358979323846f));
	}
}

void Camera::decrease_fov(float dt) {
	float fov = atan(right.length() / forward.length()) * 2 * 180 / 3.14159265358979323846f;
	if (fov - dt > 0) {
		fov = fov - dt;
		forward = forward.normalize() * (right.length() / tan(fov / 2 / 180 * 3.14159265358979323846f));
	}
}

void Camera::increase_focus_distance(float dt) {
	focus_distance = focus_distance + dt;
}

void Camera::decrease_focus_distance(float dt) {
	if (focus_distance - dt > 0)
		focus_distance = focus_distance - dt;
}

void Camera::set_focus_distance(float d) {
	focus_distance = d;
}

Ray Camera::get_ray(int x, int y) {
	Float3 right = this->right * (2.0f * x / width - 1);
	Float3 up = this->up * (2.0f * y / height - 1);

	Float3 pos_on_screen = pos + forward + right + up;
	Float3 dir = (pos_on_screen - pos).normalize();

	return Ray(pos, dir);
}

void Camera::look_at(std::vector<std::string> params) {
	if (params.size() == 3) {
		std::string var = "[" + params[0] + "," + params[1] + "," + params[2] + "]";
		Float3 look;
		std::stringstream(var) >> look;

		Float3 f = forward.normalize();
		Float3 l = (look - pos).normalize();

		Float3 h_f = Float3(f[0], 0, f[2]).normalize();
		Float3 h_l = Float3(l[0], 0, l[2]).normalize();

		float yaw = atan2(h_l[2], h_l[0]) - atan2(h_f[2], h_f[0]);
		yaw = yaw / 3.141592654f * 180.0f;
		this->rotate(0, -yaw);

		f = forward.normalize();
		l = (look - pos).normalize();

		float pitch = f.dot(l);
		if (pitch < 1) {
			pitch = acos(pitch) / 3.141592654f * 180.0f;
			pitch = std::copysignf(pitch, f[1] - l[1]);
			rotate(pitch, 0);
		}
	} else {
		GLConsole::cout << "Function takes 3 variable!\n";
	}
}

void Camera::set_pos(std::vector<std::string> params) {
	if (params.size() == 3) {
		std::string var = "[" + params[0] + "," + params[1] + "," + params[2] + "]";
		std::stringstream(var) >> pos;
	} else {
		GLConsole::cout << "Function takes 3 variable!\n";
	}
}