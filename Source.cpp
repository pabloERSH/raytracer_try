#include"vec.h"
#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#define PI 3.141592653589793


struct sphere {
private:
	vec3f center;
	float radius;
public:
	sphere(const vec3f &c, const float &r) : center(c), radius(r) {}

	bool ray_intersect(const vec3f& orig, const vec3f& dir, float& t0) const {
		
		/*vec3f L = center - orig;
		float tca = L * dir;
		float d2 = L * L - tca * tca;
		if (d2 > radius * radius) return false;
		float thc = sqrtf(radius * radius - d2);
		t0 = tca - thc;
		float t1 = tca + thc;
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return false;
		return true;*/
		
		vec3f s = center - orig;
		float ds = dir * s;
		float d = 4 * ds * ds - 4 * (dir * dir) * (s * s - radius * radius);
		if (d < 0) return false; 
		return true;

	}
};

vec3f cast_ray(const vec3f& orig, const vec3f& dir, const sphere& sphere) {

	float sphere_dist = std::numeric_limits<float>::max();
	if (!sphere.ray_intersect(orig, dir, sphere_dist)) {
		return vec3f(1, 1, 1);
	}
	return vec3f(0, 0, 0);
}

void render(const sphere& sph) {
	const int width = 1024;
	const int height = 768;
	const double fov = PI / 2.;
	std::vector<vec3f> framebuffer(width * height);

	#pragma omp parallel for
	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			float x = (2 * (i + 0.5) / (float)width - 1) * tan(fov / 2.) * width / (float)height;
			float y = -(2 * (j + 0.5) / (float)height - 1) * tan(fov / 2.);
			vec3f dir = vec3f(x, y, -1).normalize();
			framebuffer[i + j * width] = cast_ray(vec3f(0, 0, 0), dir, sph);

			//framebuffer[i + j * width] = vec3f(i/ float(width), 255, j / float(height));
		}
	}

	std::ofstream ofs;
	ofs.open("spheres.ppm", std::ofstream::out | std::ofstream::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t i = 0; i < height * width; ++i) {
		for (size_t j = 0; j < 3; j++) {
			ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
		}
	}
	ofs.close();
}

int main() {

	sphere sph(vec3f(-3, 0, -16), 2);
	render(sph);
	
	return 0;
}
