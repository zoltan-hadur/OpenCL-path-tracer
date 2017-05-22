#pragma once

#include "float3.h"
#include "Ray.h"

class Camera {
private:
	float3 pos, right, up, forward;
	float focus_distance, fov;
	int width, height;
public:
	Camera(int width, int height);				// Initializes the camera.
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
};

Camera::Camera(int width = 600, int height = 600) {
	focus_distance = 1000.0f;
	fov = 90.0f;
	this->width = width;
	this->height = height;

	float right_length = width / 2.0f;
	float up_length = height / 2.0f;
	float forward_length = right_length / tan(fov / 2.0f / 180.0f * 3.14159265358979323846f);

	pos = float3(800.0f, 500.0f, -1500.0f);
	right = float3(right_length, 0.0f, 0.0f);
	up = float3(0.0f, up_length, 0.0f);
	forward = float3(0.0f, 0.0f, forward_length);
}

void Camera::rotate(float dpitch, float dyaw) {
	float3 y_axis = float3(0, 1, 0);

	right = float3::rodrigues(right, y_axis, dyaw);
	up = float3::rodrigues(up, y_axis, dyaw);
	forward = float3::rodrigues(forward, y_axis, dyaw);

	up = float3::rodrigues(up, right.normalize(), dpitch);
	forward = float3::rodrigues(forward, right.normalize(), dpitch);
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
	float3 right = this->right * (2.0f * x / width - 1);
	float3 up = this->up * (2.0f * y / height - 1);

	float3 pos_on_screen = pos + forward + right + up;
	float3 dir = (pos_on_screen - pos).normalize();

	return Ray(pos, dir);
}