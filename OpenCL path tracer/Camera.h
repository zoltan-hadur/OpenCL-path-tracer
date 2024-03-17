#pragma once

#include <sstream>
#include "Vector3.h"
#include "Ray.h"
#include "GLConsole.h"

#pragma pack(1)
__declspec(align(16))
class Camera {
private:
	Vector3 pos;									// Position of the eye
	Vector3 right;								// X axis of the camera's coordinate system
	Vector3 up;									// Y axis of the camera's coordinate system
	Vector3 forward;								// Z axis of the camera's coordinate system
	cl_float focus_distance;					// Focus distance
	cl_float fov;								// Horizontal field of vision in °
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
#pragma pack()

Camera::Camera(cl_uint width = 600, cl_uint height = 600) {
	focus_distance = 1000.0f;
	fov = 90.0f;
	this->width = width;
	this->height = height;

	float right_length = width / 2.0f;
	float up_length = height / 2.0f;
	float forward_length = right_length / tan(fov / 2.0f / 180.0f * 3.14159265358979323846f);

	pos = Vector3(800.0f, 500.0f, -1500.0f);
	right = Vector3(right_length, 0.0f, 0.0f);
	up = Vector3(0.0f, up_length, 0.0f);
	forward = Vector3(0.0f, 0.0f, forward_length);
}

void Camera::rotate(float dpitch, float dyaw) {
	Vector3 y_axis = Vector3(0, 1, 0);

	right = Vector3::Rotate(right, y_axis, dyaw);
	up = Vector3::Rotate(up, y_axis, dyaw);
	forward = Vector3::Rotate(forward, y_axis, dyaw);

	up = Vector3::Rotate(up, right.Normalize(), dpitch);
	forward = Vector3::Rotate(forward, right.Normalize(), dpitch);
}

void Camera::translate_right(float v) {
	pos = pos + right.Normalize() * v;
}

void Camera::translate_up(float v) {
	pos = pos + up.Normalize() * v;
}

void Camera::translate_forward(float v) {
	pos = pos + forward.Normalize() * v;
}

void Camera::increase_fov(float dt) {
	float fov = atan(right.Length() / forward.Length()) * 2 * 180 / 3.14159265358979323846f;
	if (fov + dt < 175) {
		fov = fov + dt;
		forward = forward.Normalize() * (right.Length() / tan(fov / 2 / 180 * 3.14159265358979323846f));
	}
}

void Camera::decrease_fov(float dt) {
	float fov = atan(right.Length() / forward.Length()) * 2 * 180 / 3.14159265358979323846f;
	if (fov - dt > 0) {
		fov = fov - dt;
		forward = forward.Normalize() * (right.Length() / tan(fov / 2 / 180 * 3.14159265358979323846f));
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
	Vector3 right = this->right * (2.0f * x / width - 1);
	Vector3 up = this->up * (2.0f * y / height - 1);

	Vector3 pos_on_screen = pos + forward + right + up;
	Vector3 dir = (pos_on_screen - pos).Normalize();

	return Ray(pos, dir);
}

void Camera::look_at(std::vector<std::string> params) {
	if (params.size() == 3) {
		std::string var = "[" + params[0] + "," + params[1] + "," + params[2] + "]";
		Vector3 look;
		std::stringstream(var) >> look;

		Vector3 f = forward.Normalize();
		Vector3 l = (look - pos).Normalize();

		Vector3 h_f = Vector3(f[0], 0, f[2]).Normalize();
		Vector3 h_l = Vector3(l[0], 0, l[2]).Normalize();

		float yaw = atan2(h_l[2], h_l[0]) - atan2(h_f[2], h_f[0]);
		yaw = yaw / 3.141592654f * 180.0f;
		this->rotate(0, -yaw);

		f = forward.Normalize();
		l = (look - pos).Normalize();

		float pitch = f.Dot(l);
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