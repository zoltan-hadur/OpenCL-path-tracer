typedef struct {
	float3 kd, ks, emission;				// Diffuse, specular, emission
	float R0, n, shininess, glossiness;	// Fresnel (Schlick's approximation), refractive index, shininess
	char refractive;
}Material;

typedef struct {
	float width, height;
	short index;			// Index to texture
	char flag;			// 0-nothing, 1-texture, 2-bump map
}TextureInfo;

typedef struct {
	float3 origin, direction;
}Ray;

typedef struct {
	float t;
	float3 pos, norm;
	float u, v;
	short mati, texi;
}Hit;

typedef struct {
	float3 pos, orientation;	// Coordinate of the center, orientation
	float r;					// Radius
	short mati, texi;		// Index to material, index to texture info
}Sphere;

typedef struct {
	float3 v1, v2, v3;		// Vertices
	short mati, texi;	// Index to material, index to texture info
}Triangle;

typedef struct {
	float3 pos, right, up, forward;
	float focus_distance, fov;
	int width, height;
}Camera;

Ray cons_ray(float3 origin, float3 direction) {
	Ray ray; ray.origin = origin; ray.direction = direction; return ray;
}

float rand(global int* seed) {
	ulong n = (*seed);
	n = (n * 48271) % 2147483647;
	(*seed) = n;
	return n / 2147483647.0f;
}

float3 sRGB(float3 c) {
	float a[3];
	a[0] = c.x; a[1] = c.y; a[2] = c.z;
	for (int i = 0; i < 3; ++i) {
		if (a[i] <= 0.00304f) {
			a[i] = 12.92f*a[i];
		} else {
			a[i] = 1.055f*pow(a[i], 0.4167f) - 0.055f;
		}
	}
	return (float3)(a[0], a[1], a[2]);
}
float4 reinhard_tone_map(float3 c) {
	float L = 0.2126f*c.x + 0.7152f*c.y + 0.0722f*c.z;
	float L2 = L / (1 + L);
	c = c*L2 / L;
	return (float4)(sRGB(c), 1.0f);
}
float4 filmic_tone_map(float3 c) {
	c = max(0.0f, c - 0.004f);
	c = (c*(c*6.2f + 0.5f)) / (c*(c*6.2f + 1.7f) + 0.06f);
	return (float4)(c, 1.0f);
}

void kernel generate_primary_rays(global Ray* rays, global int* randoms, const Camera camera) {
	int id = get_global_id(1)*get_global_size(0) + get_global_id(0);

	int idx = get_global_id(0);
	int idy = get_global_id(1);

	float x = (float)get_global_id(0) + rand(&randoms[id]);
	float y = (float)get_global_id(1) + rand(&randoms[id]);

	float3 right = camera.right * (2.0f * x / camera.width - 1);
	float3 up = camera.up    * (2.0f * y / camera.height - 1);

	float3 pos_on_screen = camera.pos + camera.forward + right + up;
	float3 dir = normalize(pos_on_screen - camera.pos);

	pos_on_screen = camera.pos + dir * camera.focus_distance;

	float a = 2.0f * M_PI * rand(&randoms[id]);
	float r = sqrt(rand(&randoms[id]));
	float radius = 5;
	x = radius*r*cos(a);
	y = radius*r*sin(a);

	float3 camera_pos = camera.pos + normalize(camera.right)*x + normalize(camera.up)*y;

	dir = normalize(pos_on_screen - camera_pos);

	rays[id] = cons_ray(camera_pos, dir);
}

void kernel draw_screen(write_only image2d_t canvas, global float3* radiances, const int mode) {
	int id = get_global_id(1)*get_global_size(0) + get_global_id(0);

	switch (mode) {
		case 0:write_imagef(canvas, (int2)(get_global_id(0), get_global_id(1)), (float4)(radiances[id], 1));
			break;
		case 1:write_imagef(canvas, (int2)(get_global_id(0), get_global_id(1)), (float4)(sRGB(radiances[id]), 1));
			break;
		case 2:write_imagef(canvas, (int2)(get_global_id(0), get_global_id(1)), reinhard_tone_map(radiances[id]));
			break;
		case 3:write_imagef(canvas, (int2)(get_global_id(0), get_global_id(1)), filmic_tone_map(radiances[id]));
			break;
	}
}




float sRGB_to_linear(float x) {
	if (x < 0.04045f) {
		return x / 12.92f;
	} else {
		return pow((x + 0.055f) / 1.055f, 2.4f);
	}
}

float linear_to_sRGB(float y) {
	if (y <= 0.0031308f) {
		return y*12.92f;
	} else {
		return pow(y, 1.0f / 2.4f)*1.055f - 0.055f;
	}
}

inline int mod(int i, int i_max) {
	return ((i%i_max) + i_max) % i_max;
}

void kernel bgr_to_rgb(global uchar* image_in, global float* image_out) {
	int id = get_global_id(0);

	image_out[id * 4 + 0] = image_in[id * 3 + 2] / 255.0f;	// R
	image_out[id * 4 + 1] = image_in[id * 3 + 1] / 255.0f;	// G
	image_out[id * 4 + 2] = image_in[id * 3 + 0] / 255.0f;	// B
	image_out[id * 4 + 3] = 1.0f;							// A
}

void kernel rgb_to_grayscale(global float* image_in, global float* image_out) {
	int id = get_global_id(0);

	float R = sRGB_to_linear(image_in[id * 4 + 0]);	// R
	float G = sRGB_to_linear(image_in[id * 4 + 1]);	// G
	float B = sRGB_to_linear(image_in[id * 4 + 2]);	// B

	image_out[id] = linear_to_sRGB(0.2126f * R + 0.7152f * G + 0.0722f * B);
}

void kernel derivate_image(global float* image_in, global float* image_out, int width, int height) {
	int id = get_global_id(0);
	int x = id % width;
	int y = id / width;

	int xp = mod(x + 1, width);
	int xm = mod(x - 1, width);
	int yp = mod(y + 1, height);
	int ym = mod(y - 1, height);

	float horizontal = image_in[y * width + xp] - image_in[y * width + xm];
	float vertical = image_in[yp * width + x] - image_in[ym * width + x];

	image_out[id * 4 + 0] = tanh(2.0f * horizontal) / 2.0f + 0.5f;
	image_out[id * 4 + 1] = tanh(2.0f * vertical) / 2.0f + 0.5f;
	image_out[id * 4 + 2] = 1.0f;
	image_out[id * 4 + 3] = 1.0f;
}

void kernel expand_image(global float* image_in, global float* image_out, int width, int height) {
	int id = get_global_id(0);
	int x = id % 2048;
	int y = id / 2048;
	image_out[y * 2048 * 4 + x * 4 + 0] = image_in[mod(y, height)*width * 4 + mod(x, width) * 4 + 0];
	image_out[y * 2048 * 4 + x * 4 + 1] = image_in[mod(y, height)*width * 4 + mod(x, width) * 4 + 1];
	image_out[y * 2048 * 4 + x * 4 + 2] = image_in[mod(y, height)*width * 4 + mod(x, width) * 4 + 2];
	image_out[y * 2048 * 4 + x * 4 + 3] = 1;
}