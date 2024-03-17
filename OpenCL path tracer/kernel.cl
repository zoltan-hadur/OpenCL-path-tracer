//enum ToneMap {
//	RAW, SRGB, REINHARD, FILMIC, ToneMap_COUNT
//};
//enum Mode {
//	RAY_TRACING, PATH_TRACING, EXPLICIT_LIGHT_SAMPLING, Mode_COUNT
//};
//enum Texture {
//	NOTHING, HAS_TEXTURE, HAS_BUMP_MAP
//};

#define RAW 0
#define SRGB 1
#define REINHARD 2
#define FILMIC 3

#define RAY_TRACING 0
#define PATH_TRACING 1
#define EXPLICIT_LIGHT_SAMPLING 2

#define NOTHING 0
#define HAS_TEXTURE 1
#define HAS_BUMP_MAP 2

typedef struct Material {
	float3 kd;			// Diffuse color
	float3 ks;			// Specular color
	float3 emission;	// Emission
	//float R0;			// Reflectance at 0°
	float n;			// Refractive index
	float glossiness;	// Glossiness
	uint type;			// 0-emitter, 1-diffuse or specular, 2-refractive
} __attribute__ ((aligned(16),packed)) Material;

typedef struct TextureInfo {
	float width;		// Width of the texture
	float height;		// Height of the texture
	uint index;			// Index to the texture
	uint flag;			// 0-nothing, 1-texture, 2-bump map
} __attribute__ ((aligned(4),packed)) TextureInfo;

typedef struct Ray {
	float3 pos;			// Origin of the ray
	float3 dir;			// Direction of the ray
} __attribute__ ((aligned(16),packed)) Ray;

typedef struct Hit {
	float3 pos;			// Position of the intersection
	float3 norm;		// Normal vector at the intersection point
	float t;			// Time
	float u;			// U texture coordinate
	float v;			// V texture coordinate
	uint mati;			// Index to the material
	uint texi;			// Index to the texture info
} Hit;

typedef struct Sphere {
	float3 pos;			// Coordinate of the center
	float3 ori;			// Orientation
	float r;			// Radius
	uint mati;			// Index to the material
	uint texi;			// Index to the texture info
} __attribute__ ((aligned(16),packed)) Sphere;

typedef struct Triangle {
	float3 v1;			// Vertices
	float3 v2;			//
	float3 v3;			// 
	uint mati;			// Index to the material
	uint texi;			// Index to the texture info
} __attribute__ ((aligned(16),packed)) Triangle;

typedef struct Camera {
	float3 pos;				// Position of the eye
	float3 right;			// X axis of the camera's coordinate system
	float3 up;				// Y axis of the camera's coordinate system
	float3 forward;			// Z axis of the camera's coordinate system
	float focus_distance;	// Focus distance
	float fov;				// Horizontal field of vision in °
	uint width;				// Width of the canvas
	uint height;			// Height of the canvas
} __attribute__ ((aligned(16),packed)) Camera;

Ray cons_ray(float3 pos, float3 dir) {
	Ray ray; ray.pos = pos; ray.dir = dir; return ray;
}

Hit cons_hit(float t, float3 pos, float3 norm, float u, float v, uint mati, uint texi) {
	Hit hit; hit.t = t; hit.pos = pos; hit.norm = norm; hit.u = u; hit.v = v; hit.mati = mati; hit.texi = texi; return hit;
}

Hit init_hit() {
	return cons_hit(-1.0f, (float3)(0.0f, 0.0f, 0.0f), (float3)(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0, 0);
}

float rand(global uint* seed) {
	ulong n = (*seed);
	n = (n * 48271) % 2147483647;
	(*seed) = n;
	return n / 2147483647.0f;
}

float3 rodrigues(float3 v, float3 k, float theta) {
	theta = theta / 180.0f * M_PI;
	float cost = cos(theta);
	float sint = sin(theta);
	return v*cost + cross(k, v)*sint + k*dot(k, v)*(1 - cost);
}

void ons(float3* w, float3* u, float3* v) {
	float3 X = (float3)(1, 0, 0);
	float3 Y = (float3)(0, 1, 0);
	float3 Z = (float3)(0, 0, 1);

	//float3 perp = normalize(cross(Y, *w));
	//float cosa = dot(Y, *w);
	//float deg = acos(cosa) / M_PI * 180.0f;
	//X = rodrigues(X, perp, deg);
	//Z = rodrigues(Z, perp, deg);
	//*u = X;
	//*v = Z;

	if ((*w).y >= 0.999) {
		*u = X;
		*v = Z;
	} else if ((*w).y <= -0.999) {
		*w = -Y;
		*u = -X;
		*v = -Z;
	} else {
		float3 perp = normalize(cross(Y, *w));
		float cosa = dot(Y, *w);
		float deg = acos(cosa) / M_PI * 180.0f;
		X = rodrigues(X, perp, deg);
		Z = rodrigues(Z, perp, deg);
		*u = X;
		*v = Z;
	}
}

float fresnel(float3 norm, float n, Ray ray) {
	float R0 = ((1 - n) / (1 + n)) * ((1 - n) / (1 + n));
	float cosa = fabs(dot(norm, ray.dir));
	return R0 + (1 - R0)*pow(1 - cosa, 5);
}

bool intersect_sphere(Sphere sphere, Ray ray, Hit* hit) {
	//solving |ray(t) - sphere.pos|^2 = sphere.r^2 for t
	//
	//dot(ray.dir, ray.dir)*t^2 + 2*dot(ray.pos-sphere.pos, ray.dir)*t + dot(ray.pos-sphere.pos, ray.pos-sphere.pos) - sphere.r^2 = 0
	//          a          *t^2 +                  b                *t +                            c                             = 0
	//if ray.dir is normalized (and it is), then dot(ray.dir, ray.dir) = 1 -> a = 1
	float b = 2.0f * dot(ray.pos - sphere.pos, ray.dir);
	float c = dot(ray.pos - sphere.pos, ray.pos - sphere.pos) - sphere.r*sphere.r;

	//discriminant = b^2 - 4ac
	float d = b*b - 4.0f * c;

	//if the discriminant < 0, then the ray does not intersect the sphere
	if (d < 0.0f) {
		return false;
	}

	//solution of the quadratic equation
	//t1 = (-b - sqrt(b^2 -4ac))/(2a)
	//t2 = (-b + sqrt(b^2 -4ac))/(2a)
	//
	//t1 is always smaller then t2
	//if t1 is positive, then ray.pos + ray.dir*t1 is the closest intersection in front of the ray origin
	//else, if t2 is positive, then we are in the sphere, and ray.pos + ray.dir*t1 is behind the ray oirigin,
	//thus ray.pos + ray.dir*t2 will be the closest intersection in front of the ray origin
	//if both t1 and t2 are negative, then the however the ray intersects the sphere, the entire sphere is behind the ray origin
	//and it is not visible to us
	d = sqrt(d);
	float t1 = (-b - d) / 2.0f;
	float t2 = (-b + d) / 2.0f;
	float t;
	if (t1 > 0.001f) {
		t = t1;
	} else if (t2 > 0.001f) {
		t = t2;
	} else {
		return false;
	}
	float3 hit_pos = ray.pos + ray.dir*t;
	float3 hit_norm = (hit_pos - sphere.pos) / sphere.r;

	// Calculate texture coordinates according to orientation
	float3 X = (float3)(1, 0, 0);
	float3 Y = (float3)(0, 1, 0);
	float3 Z = (float3)(0, 0, 1);
	float3 right = X;
	float3 up = Y;
	float3 forward = Z;

	up = rodrigues(up, up, sphere.ori.x);
	right = rodrigues(right, up, sphere.ori.x);
	forward = rodrigues(forward, up, sphere.ori.x);

	up = rodrigues(up, right, sphere.ori.y);
	right = rodrigues(right, right, sphere.ori.y);
	forward = rodrigues(forward, right, sphere.ori.y);

	up = rodrigues(up, up, sphere.ori.z);
	right = rodrigues(right, up, sphere.ori.z);
	forward = rodrigues(forward, up, sphere.ori.z);

	//right = rodrigues(right, Y, sphere.ori.x);
	//right = rodrigues(right, X, sphere.ori.y);
	//right = rodrigues(right, Y, sphere.ori.z);
	//up = rodrigues(up, Y, sphere.ori.x);
	//up = rodrigues(up, X, sphere.ori.y);
	//up = rodrigues(up, Y, sphere.ori.z);
	//forward = rodrigues(forward, Y, sphere.ori.x);
	//forward = rodrigues(forward, X, sphere.ori.y);
	//forward = rodrigues(forward, Y, sphere.ori.z);

	float cosr = dot(right, hit_norm);
	float cosf = dot(forward, hit_norm);
	float u = -atan2pi(copysign(length(right*cosr), cosr), copysign(length(forward*cosf), cosf)) / 2.0f + 0.5f;
	float v = dot(hit_norm, up) / 2.0f + 0.5f;

	*hit = cons_hit(t, hit_pos, hit_norm, u, v, sphere.mati, sphere.texi);
	return true;
}

bool intersect_triangle(Triangle triangle, Ray ray, Hit* hit) {
	//Find vectors for two edges sharing V1
	float3 e1 = triangle.v2 - triangle.v1;
	float3 e2 = triangle.v3 - triangle.v1;
	//Begin calculating determinant - also used to calculate u parameter
	float3 P = cross(ray.dir, e2);
	//if determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
	float det = dot(e1, P);
	//NOT CULLING
	if (det > -0.000001f && det < 0.000001f) {
		return false;
	}
	float inv_det = 1.0f / det;

	//calculate distance from V1 to ray origin
	float3 T = ray.pos - triangle.v1;

	//Calculate u parameter and test bound
	float u = dot(T, P) * inv_det;
	//The intersection lies outside of the triangle
	if (u < 0.f || u > 1.f) {
		return false;
	}

	//Prepare to test v parameter
	float3 Q = cross(T, e1);

	//Calculate V parameter and test bound
	float v = dot(ray.dir, Q) * inv_det;
	//The intersection lies outside of the triangle
	if (v < 0.f || u + v  > 1.f) {
		return false;
	}

	float t = dot(e2, Q) * inv_det;

	if (t > 0.000001f) { //ray intersection
		float3 hit_pos = ray.pos + ray.dir*t;
		float3 hit_norm = normalize(cross(e1, e2));
		*hit = cons_hit(t, hit_pos, hit_norm, u, v, triangle.mati, triangle.texi);
		return true;
	}

	// No hit, no win
	return false;
}

Hit first_intersect(const uint size,
					constant Sphere* spheres,
					const uint size_spheres,
					constant Triangle* triangles,
					const uint size_triangles,
					Ray ray) {

	Hit best_hit = init_hit();
	Hit hit = init_hit();
	for (int i = 0; i < size; ++i) {
		if (i < size_spheres) {
			if (intersect_sphere(spheres[i], ray, &hit)) {
				if (best_hit.t < 0 || hit.t < best_hit.t) {
					best_hit = hit;
				}
			}
		}
		if (i < size_triangles) {
			if (intersect_triangle(triangles[i], ray, &hit)) {
				if (best_hit.t < 0 || hit.t < best_hit.t) {
					best_hit = hit;
				}
			}
		}
	}
	return best_hit;
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
	float Y = 0.2126f*c.x + 0.7152f*c.y + 0.0722f*c.z;
	float L = Y / (1 + Y);
	c = c * L / Y;
	return (float4)(sRGB(c), 1.0f);
}
float4 filmic_tone_map(float3 c) {
	c = max(0.0f, c - 0.004f);
	c = (c*(c*6.2f + 0.5f)) / (c*(c*6.2f + 1.7f) + 0.06f);
	return (float4)(c, 1.0f);
}

void kernel generate_primary_rays(global Ray* rays,
								  global uint* randoms,
								  const Camera camera) {

	uint id = get_global_id(1)*get_global_size(0) + get_global_id(0);

	float x = (float)get_global_id(0) + rand(&randoms[id]);
	float y = (float)get_global_id(1) + rand(&randoms[id]);

	float3 right = camera.right * (2.0f * x / camera.width - 1);
	float3 up = camera.up    * (2.0f * y / camera.height - 1);

	float3 pos_on_screen = camera.pos + camera.forward + right + up;
	float3 dir = normalize(pos_on_screen - camera.pos);

	pos_on_screen = camera.pos + dir * camera.focus_distance;

	float a = 2.0f * M_PI * rand(&randoms[id]);
	float r = sqrt(rand(&randoms[id]));
	float radius = 0;
	x = radius*r*cos(a);
	y = radius*r*sin(a);

	float3 camera_pos = camera.pos + normalize(camera.right)*x + normalize(camera.up)*y;

	dir = normalize(pos_on_screen - camera_pos);

	rays[id] = cons_ray(camera_pos, dir);
}

void kernel trace_rays(read_only image2d_array_t textures,
					   read_only image2d_array_t bump_maps,
					   constant TextureInfo* texture_infos,
					   constant Sphere* spheres,
					   const uint size_spheres,
					   constant Triangle* triangles,
					   const uint size_triangles,
					   constant Material* materials,
					   global Ray* rays,
					   global uint* randoms,
					   global float3* radiances,
					   const uint sample_id,
					   const uint max_depth,
					   const uint mode) {

	const sampler_t samplerA = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_REPEAT | CLK_FILTER_LINEAR;
	uint id = get_global_id(1)*get_global_size(0) + get_global_id(0);
	float3 factor = (float3)(1, 1, 1);
	float3 radiance = (float3)(0, 0, 0);
	bool in = false;
	bool sampled = false;

	uint size = max(size_spheres, size_triangles);

	for (int i = 0; i < max_depth; ++i) {
		Hit hit = first_intersect(size, spheres, size_spheres, triangles, size_triangles, rays[id]);
		hit.norm = dot(hit.norm, rays[id].dir) < 0.0f ? hit.norm : hit.norm * (-1.0f);
		Material mat = materials[hit.mati];
		TextureInfo tex_info = texture_infos[hit.texi];

		if (tex_info.flag & HAS_TEXTURE) {
			float4 tex_color = read_imagef(textures, samplerA, (float4)(hit.u*tex_info.width, hit.v*tex_info.height, tex_info.index, 0));
			mat.kd = (float3)(tex_color.x, tex_color.y, tex_color.z);
			mat.ks = mat.kd;
		}
		if (tex_info.flag & HAS_BUMP_MAP) {
			float4 tex_norm = read_imagef(bump_maps, samplerA, (float4)(hit.u*tex_info.width, hit.v*tex_info.height, tex_info.index, 0));
			float3 w = hit.norm;
			float3 u, v;
			ons(&w, &u, &v);
			hit.norm = hit.norm + (tex_norm.y - 0.5f)*2.0f*cross(u, w) + (tex_norm.x - 0.5f)*2.0f*cross(v, w);
			hit.norm = normalize(hit.norm);
			hit.norm = dot(hit.norm, rays[id].dir) < 0.0f ? hit.norm : hit.norm * (-1.0f);
		}

		if (hit.t > 0) {
			switch (mat.type) {
				case 0:
				{
					if (!sampled) {
						radiance = radiance + mat.emission * factor;
					}
					i = 999;
				}
				break;
				case 1:
				{
					float R = fresnel(hit.norm, mat.n, rays[id]);	// Probability of reflecting ray
					if (rand(&randoms[id]) < R) {
						// Perfect reflection dir
						float3 new_dir = normalize(rays[id].dir - hit.norm*dot(hit.norm, rays[id].dir)*2.0f);

						// Compute two random numbers to pick a random point on the hemisphere above the hitpoint
						float rand1 = 2.0f * M_PI * rand(&randoms[id]);
						float rand2 = rand(&randoms[id]) * mat.glossiness;
						float rand2s = sqrt(rand2);
						// Create a local orthogonal coordinate frame centered at the hitpoint
						float3 w = new_dir;
						float3 u, v;
						ons(&w, &u, &v);

						// Use the coordinte frame and random numbers to compute the next ray direction
						new_dir = normalize(u * cos(rand1)*rand2s + v*sin(rand1)*rand2s + w*sqrt(1.0f - rand2));

						Ray ray_to = cons_ray(hit.pos + hit.norm*0.01f, new_dir);

						float3 S = mat.ks * fmax(0.0f, dot(normalize(ray_to.dir - rays[id].dir), hit.norm));
						factor = factor * S / R;

						rays[id] = ray_to;
						sampled = false;
					} else {
						if (mode == RAY_TRACING || mode == EXPLICIT_LIGHT_SAMPLING) {
							float width = 398.0f;
							float3 light_pos = (float3)(601.0f + width*rand(&randoms[id]), 999.99, 1.0f + width*rand(&randoms[id]));
							float r = length(light_pos - hit.pos);
							float A = width*width;
							float3 light_dir = normalize(light_pos - hit.pos);
							Ray shadow_ray = cons_ray(hit.pos + hit.norm*0.01f, light_dir);
							Hit shadow_hit = first_intersect(size, spheres, size_spheres, triangles, size_triangles, shadow_ray);
							float3 LD = mat.kd * fmax(0.0f, dot(shadow_ray.dir, hit.norm));
							float f = A / (r*r) * dot((float3)(0, -1, 0), -light_dir);
							radiance = radiance + materials[shadow_hit.mati].emission*LD*f*factor / (1 - R);
							if (materials[shadow_hit.mati].type == 0) {
								sampled = true;
							} else {
								sampled = false;
							}
							if (mode == RAY_TRACING) {
								i = 999;
							}
						}

						if (mode == PATH_TRACING || mode == EXPLICIT_LIGHT_SAMPLING) {
							// Compute two random numbers to pick a random point on the hemisphere above the hitpoint
							float rand1 = 2.0f * M_PI * rand(&randoms[id]);
							float rand2 = rand(&randoms[id]);
							float rand2s = sqrt(rand2);
							// Create a local orthogonal coordinate frame centered at the hitpoint
							float3 w = hit.norm;
							float3 u, v;
							ons(&w, &u, &v);

							// Use the coordinte frame and random numbers to compute the next ray direction
							float3 new_dir = normalize(u * cos(rand1)*rand2s + v*sin(rand1)*rand2s + w*sqrt(1.0f - rand2));
							// Add a very small offset to the hitpoint to prevent self intersection
							Ray ray_to = cons_ray(hit.pos + hit.norm*0.01f, new_dir);

							float3 D = mat.kd * fmax(0.0f, dot(ray_to.dir, hit.norm));
							factor = factor * D / (1 - R);

							rays[id] = ray_to;
						}
					}
				}
				break;
				case 2:
				{
					Ray ray_to;
					float3 pos, dir;
					float R = fresnel(hit.norm, mat.n, rays[id]);	// Probability of reflecting ray
					if (in) {
						mat.n = 1.0f / mat.n;
					}
					float cosa = dot(-rays[id].dir, hit.norm);
					float disc = 1.0f - (1.0f - cosa*cosa) / mat.n / mat.n;
					if (disc > 0 && rand(&randoms[id]) > R) {	// Refract
						in = !in;
						pos = hit.pos - hit.norm*0.01f;
						dir = normalize(rays[id].dir / mat.n + hit.norm*(cosa / mat.n - sqrt(disc)));
						//factor = factor * (1 - (1 - mat.ks * (1 - R))) / (1 - R);
						factor = factor * mat.ks;
					} else {									// Reflect
						pos = hit.pos + hit.norm*0.01f;
						dir = normalize(rays[id].dir - hit.norm*dot(hit.norm, rays[id].dir)*2.0f);
						//factor = factor * (1 - (1 - mat.ks * R)) / R;
						factor = factor * mat.ks;
					}
					ray_to = cons_ray(pos, dir);

					rays[id] = ray_to;
					sampled = false;
				}
				break;
			}
		} else {
			break;
		}
	}
	radiances[id] = (radiances[id] * sample_id + radiance) / (sample_id + 1);
}

void kernel draw_screen(write_only image2d_t canvas,
						global float3* radiances,
						const uint mode,
						const uint filter) {

	uint width = get_global_size(0);
	uint height = get_global_size(1);
	uint x = get_global_id(0);
	uint y = get_global_id(1);
	uint id = y * width + x;

	float3 color;
	switch (filter) {
		case RAW:
			color = radiances[id];
			break;
		case 1:
			if (0 < x && x < width && 0 < y && y < height) {
				color = (radiances[(y + 1) * width + (x - 1)] + radiances[(y + 1) * width + (x + 0)] + radiances[(y + 1) * width + (x + 1)] +
						 radiances[(y + 0) * width + (x - 1)] + radiances[(y + 0) * width + (x + 0)] + radiances[(y + 0) * width + (x + 1)] +
						 radiances[(y - 1) * width + (x - 1)] + radiances[(y - 1) * width + (x + 0)] + radiances[(y - 1) * width + (x + 1)]) / 9;
			} else {
				color = radiances[id];
			}
			break;
		case 2:
			if (0 < x && x < width && 0 < y && y < height) {
				color = (float3)(0, 0, 0);
				float3 colors[9];
				colors[0] = radiances[(y - 1) * width + (x - 1)];
				colors[1] = radiances[(y - 1) * width + (x + 0)];
				colors[2] = radiances[(y - 1) * width + (x + 1)];

				colors[3] = radiances[(y + 0) * width + (x - 1)];
				colors[4] = radiances[(y + 0) * width + (x + 0)];
				colors[5] = radiances[(y + 0) * width + (x + 1)];

				colors[6] = radiances[(y + 1) * width + (x - 1)];
				colors[7] = radiances[(y + 1) * width + (x + 0)];
				colors[8] = radiances[(y + 1) * width + (x + 1)];

				for (int i = 0; i < 9; i++) {
					for (int j = 0; j < 9 - i; j++) {
						if (colors[j].x > colors[j + 1].x) {
							float3 temp = colors[j];
							colors[j] = colors[j + 1];
							colors[j + 1] = temp;
						}
					}
				}
				color.x = colors[4].x;

				for (int i = 0; i < 9; i++) {
					for (int j = 0; j < 9 - i; j++) {
						if (colors[j].y > colors[j + 1].y) {
							float3 temp = colors[j];
							colors[j] = colors[j + 1];
							colors[j + 1] = temp;
						}
					}
				}
				color.y = colors[4].y;

				for (int i = 0; i < 9; i++) {
					for (int j = 0; j < 9 - i; j++) {
						if (colors[j].z > colors[j + 1].z) {
							float3 temp = colors[j];
							colors[j] = colors[j + 1];
							colors[j + 1] = temp;
						}
					}
				}
				color.z = colors[4].z;


			} else {
				color = radiances[id];
			}
			break;
	}

	float4 out;
	switch (mode) {
		case RAW:
			out = (float4)(color, 1);
			break;
		case SRGB:
			out = (float4)(sRGB(color), 1);
			break;
		case REINHARD:
			out = reinhard_tone_map(color);
			break;
		case FILMIC:
			out = filmic_tone_map(color);
			break;
	}

	write_imagef(canvas, (int2)(x, y), out);
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

void kernel bgr_to_rgb(global uchar* image_in,
					   global float* image_out) {

	uint id = get_global_id(0);

	image_out[id * 4 + 0] = image_in[id * 3 + 2] / 255.0f;	// R
	image_out[id * 4 + 1] = image_in[id * 3 + 1] / 255.0f;	// G
	image_out[id * 4 + 2] = image_in[id * 3 + 0] / 255.0f;	// B
	image_out[id * 4 + 3] = 1.0f;							// A
}

void kernel rgb_to_grayscale(global float* image_in,
							 global float* image_out) {

	uint id = get_global_id(0);

	float R = sRGB_to_linear(image_in[id * 4 + 0]);	// R
	float G = sRGB_to_linear(image_in[id * 4 + 1]);	// G
	float B = sRGB_to_linear(image_in[id * 4 + 2]);	// B

	image_out[id] = linear_to_sRGB(0.2126f * R + 0.7152f * G + 0.0722f * B);
}

void kernel derivate_image(global float* image_in,
						   global float* image_out,
						   uint width,
						   uint height) {

	uint id = get_global_id(0);
	uint x = id % width;
	uint y = id / width;

	uint xp = mod(x + 1, width);
	uint xm = mod(x - 1, width);
	uint yp = mod(y + 1, height);
	uint ym = mod(y - 1, height);

	float horizontal = image_in[y * width + xp] - image_in[y * width + xm];
	float vertical = image_in[yp * width + x] - image_in[ym * width + x];

	image_out[id * 4 + 0] = tanh(2.0f * horizontal) / 2.0f + 0.5f;
	image_out[id * 4 + 1] = tanh(2.0f * vertical) / 2.0f + 0.5f;
	image_out[id * 4 + 2] = 1.0f;
	image_out[id * 4 + 3] = 1.0f;
}

void kernel expand_image(global float* image_in,
						 global float* image_out,
						 uint width,
						 uint height) {

	uint id = get_global_id(0);
	uint x = id % 2048;
	uint y = id / 2048;

	image_out[y * 2048 * 4 + x * 4 + 0] = image_in[mod(y, height)*width * 4 + mod(x, width) * 4 + 0];
	image_out[y * 2048 * 4 + x * 4 + 1] = image_in[mod(y, height)*width * 4 + mod(x, width) * 4 + 1];
	image_out[y * 2048 * 4 + x * 4 + 2] = image_in[mod(y, height)*width * 4 + mod(x, width) * 4 + 2];
	image_out[y * 2048 * 4 + x * 4 + 3] = 1;
}