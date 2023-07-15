#include"vec.h"
#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#define PI 3.141592653589793

struct Light {
	Light(const vec3f& p, const float i) : position(p), intensity(i) {}
	vec3f position;
	float intensity;
};

struct Material {
	vec3f diffuse_color;
	vec4f albedo;
	float specular_exponent;
	float refractive_index;
	Material(const float r, const vec4f& a, const vec3f& color, const float spec) : refractive_index(r), albedo(a), diffuse_color(color), specular_exponent(spec) {}
	Material() : refractive_index(1), albedo(1, 0, 0, 0), diffuse_color(), specular_exponent() {}
};

struct Sphere {
	vec3f center;
	float radius;
	Material material;

	Sphere(const vec3f& c, const float r, const Material& m) : center(c), radius(r), material(m) {}

	bool ray_intersect(const vec3f& orig, const vec3f& dir, float& t0) const {

		vec3f L = center - orig;
		float tca = L * dir;
		float d2 = L * L - tca * tca;
		if (d2 > radius * radius) return false;
		float thc = sqrtf(radius * radius - d2);
		t0 = tca - thc;
		float t1 = tca + thc;
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return false;
		return true;

	}
};

vec3f reflect(const vec3f& I, const vec3f& N) {
	return I - N * 2.f * (I * N);
}

vec3f refract(const vec3f& I, const vec3f& N, const float eta_t, const float eta_i = 1.f) {
	float cosi = -std::max(-1.f, std::min(1.f, I * N));
	if (cosi < 0) return refract(I, N * -1.f, eta_i, eta_t);
	float eta = eta_i / eta_t;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? vec3f(1, 0, 0) : I * eta + N * (eta * cosi - sqrtf(k));
}

bool scene_intersect(const vec3f& orig, const vec3f& dir, const std::vector<Sphere>& spheres, vec3f& hit, vec3f& N, Material& material) {
	float spheres_dist = std::numeric_limits<float>::max();
	for (size_t i = 0; i < spheres.size(); i++) {
		float dist_i;
		if (spheres[i].ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist) {
			spheres_dist = dist_i;
			hit = orig + dir * dist_i;
			N = (hit - spheres[i].center).normalize();
			material = spheres[i].material;
		}
	}
	float checkerboard_dist = std::numeric_limits<float>::max();
	if (fabs(dir[1]) > float(1e-3)) {
		float d = -(orig[1] + 4) / dir[1];
		vec3f pt = orig + dir * d;
		if (d > 0 && fabs(pt[0]) < 15 && pt[2]<0 && pt[2]>-40 && d < spheres_dist) {
			checkerboard_dist = d;
			hit = pt;
			N = vec3f(0, 1, 0);
			material.diffuse_color = (int(.5 * hit[0] + 1000) + int(.5 * hit[2])) & 1 ? vec3f(1, 1, 1) : vec3f(0, 0, 0);
			material.diffuse_color = material.diffuse_color * float(.3);
		}
	}
	return std::min(spheres_dist, checkerboard_dist) < 1000;
}

vec3f cast_ray(const vec3f& orig, const vec3f& dir, const std::vector<Sphere>& spheres, const std::vector<Light>& lights, size_t depth = 0) {
	vec3f point, N;
	Material material;
	if (depth > 4 || !scene_intersect(orig, dir, spheres, point, N, material)) {
		return vec3f(0, 0, 0);
	}

	vec3f reflect_dir = reflect(dir, N);
	vec3f refract_dir = refract(dir, N, material.refractive_index).normalize();
	vec3f reflect_orig = reflect_dir * N < 0 ? point - N * float(1e-3) : point + N * float(1e-3);
	vec3f refract_orig = refract_dir * N < 0 ? point - N * float(1e-3) : point + N * float(1e-3);
	vec3f reflect_color = cast_ray(reflect_orig, reflect_dir, spheres, lights, depth + 1);
	vec3f refract_color = cast_ray(refract_orig, refract_dir, spheres, lights, depth + 1);


	float diffuse_light_intensity = 0, specular_light_intensity = 0;
	for (size_t i = 0; i < lights.size(); i++) {
		vec3f light_dir = (lights[i].position - point).normalize();

		float light_distance = (lights[i].position - point).norm();
		vec3f shadow_orig = light_dir * N < 0 ? point - N * float(1e-3) : point + N * float(1e-3);
		vec3f shadow_pt, shadow_N;
		Material tmpmaterial;
		if (scene_intersect(shadow_orig, light_dir, spheres, shadow_pt, shadow_N, tmpmaterial) && (shadow_pt - shadow_orig).norm() < light_distance) continue;

		diffuse_light_intensity += lights[i].intensity * std::max(0.f, light_dir * N);
		specular_light_intensity += powf(std::max(0.f, reflect(light_dir, N) * dir), material.specular_exponent) * lights[i].intensity;
	}
	return material.diffuse_color * diffuse_light_intensity * material.albedo[0] + vec3f(1., 1., 1.) * specular_light_intensity * material.albedo[1] + reflect_color * material.albedo[2] + refract_color * material.albedo[3];
}

void render(const std::vector<Sphere>& spheres, const std::vector<Light>& lights) {
	const int width = 1920;
	const int height = 1080;
	const double fov = PI / 2.;
	std::vector<vec3f> framebuffer(width * height);
#pragma omp parallel for
	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			float dir_x = (i + 0.5) - width / 2.;
			float dir_y = -(j + 0.5) + height / 2.;
			float dir_z = -height / (2. * tan(fov / 2.));
			framebuffer[i + j * width] = cast_ray(vec3f(0, 0, 0), vec3f(dir_x, dir_y, dir_z).normalize(), spheres, lights);
		}
	}


	std::ofstream ofs;
	ofs.open("spheres.ppm", std::ofstream::out | std::ofstream::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t i = 0; i < height * width; ++i) {
		vec3f& c = framebuffer[i];
		float max = std::max(c[0], std::max(c[1], c[2]));
		if (max > 1) c = c * float((1. / max));
		for (size_t j = 0; j < 3; j++) {
			ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
		}
	}
	ofs.close();
}

int main() {

	Material carbon(1.0, vec4f(0.6, 0.3, 0.0, 0.0), vec3f(0.188, 0.188, 0.188), 50.);
	Material aluminium(1.0, vec4f(0.9, 0.1, 0.1, 0.0), vec3f(0.8, 0.8, 0.8), 10.);
	Material mirror(1.0, vec4f(0.0, 10.0, 0.8, 0.0), vec3f(1.0, 1.0, 1.0), 1425.);
	Material glass(1.5, vec4f(0.0, 0.5, 0.1, 0.8), vec3f(0.6, 0.7, 0.8), 125.);

	std::vector<Sphere> spheres;
	spheres.push_back(Sphere(vec3f(-3, -0, -16), 2, carbon));
	spheres.push_back(Sphere(vec3f(-1.0, -1.5, -12), 2, glass));
	spheres.push_back(Sphere(vec3f(1.5, -0.5, -18), 3, carbon));
	spheres.push_back(Sphere(vec3f(7, 5, -18), 4, aluminium));
	spheres.push_back(Sphere(vec3f(-6, 10, -20), 7, mirror));
	spheres.push_back(Sphere(vec3f(3, 4, -8), 1, aluminium));
	spheres.push_back(Sphere(vec3f(-2, -5, -7), 0.6, carbon));
	spheres.push_back(Sphere(vec3f(-3, -6, -10), 2, glass));
	std::vector<Light>  lights;
	lights.push_back(Light(vec3f(-20, 20, 20), 1.5));
	lights.push_back(Light(vec3f(30, 50, -25), 1.8));
	lights.push_back(Light(vec3f(30, 20, 30), 1.7));

	render(spheres, lights);


	return 0;
}
