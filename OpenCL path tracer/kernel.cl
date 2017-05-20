void kernel bgr_to_rgb(global uchar* image_in, global float* image_out) {
	int id = get_global_id(0);

	image_out[id * 4 + 0] = image_in[id * 3 + 2] / 255.0f;	// R
	image_out[id * 4 + 1] = image_in[id * 3 + 1] / 255.0f;	// G
	image_out[id * 4 + 2] = image_in[id * 3 + 0] / 255.0f;	// B
	image_out[id * 4 + 3] = 1.0f;							// A
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

void kernel rgb_to_grayscale(global float* image_in, global float* image_out) {
	int id = get_global_id(0);

	float R = sRGB_to_linear(image_in[id * 4 + 0]);	// R
	float G = sRGB_to_linear(image_in[id * 4 + 1]);	// G
	float B = sRGB_to_linear(image_in[id * 4 + 2]);	// B

	image_out[id] = linear_to_sRGB(0.2126f * R + 0.7152f * G + 0.0722f * B);
}

inline int mod(int i, int i_max) {
	return ((i%i_max) + i_max) % i_max;
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