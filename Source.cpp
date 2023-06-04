#include"vec.h"
#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

void render() {
	const int width = 2048;
	const int height = 1536;
	std::vector<vec3f> framebuffer(width * height);

	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			framebuffer[i + j * width] = vec3f(i / float(height), j / float(width), 0);
		}
	}

	std::ofstream ofs;
	ofs.open("12345jopa.ppm");
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (size_t i = 0; i < height * width; ++i) {
		for (size_t j = 0; j < 3; j++) {
			ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
		}
	}
	ofs.close();
}

int main() {
	render();
	return 0;
}
