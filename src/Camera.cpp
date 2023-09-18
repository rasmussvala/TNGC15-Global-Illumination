#include "../include/Camera.h"
#include <fstream>
#include <iostream>

Camera::Camera(int w, int h) : width(w), height(h) {
	// Allokera minne f�r pixels
	pixels.resize(height);

	for (int i = 0; i < height; ++i) {
		pixels[i].resize(width);
	}
}

int Camera::getWidth() {
	return width;
}

int Camera::getHeight() {
	return height;
}

glm::vec3 Camera::getLocation() {
	return glm::vec3();
}

void Camera::saveImage(std::string filename, int width, int height, const std::vector<std::vector<ColorRGB>>& pixels) {

	std::ofstream ppmFile(filename); // Open a file for writing

	ppmFile << "P3\n" << width << ' ' << height << "\n255\n";

	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {

			auto r = pixels[j][i].r;
			auto g = pixels[j][i].g;
			auto b = pixels[j][i].b;

			// Convert to 0-255
			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);

			ppmFile << ir << ' ' << ig << ' ' << ib << '\n';
		}
	}

	ppmFile.close();
}

void Camera::traceRays(std::vector<std::vector<ColorRGB>>& pixels, const std::vector<Polygon*>& objects) {
	// Loopar igenom alla pixlar
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {

			// Skapar en ray f�r varje pixel
			Ray ray(location, calculateRayDirection(i, j));
			glm::vec3 intersectionPoint;

			// Kollar om ray intersectar n�got objekt
			bool hit = false;
			for (const auto& obj : objects) {
				if (obj->intersect(ray, intersectionPoint)) {
					hit = true;
					break;
				}
			}

			// Ans�tter f�rgen p� pixeln 
			if (hit) {
				pixels[j][i] = ColorRGB(0.0, 1.0, 0.0); // Gr�n f�r hit
			}
			else {
				pixels[j][i] = ColorRGB(1.0, 0.0, 0.0); // R�d f�r miss
			}
		}
	}
}

glm::vec3 Camera::calculateRayDirection(int i, int j) {
	// Ber�knar u och v (positionen i world coordinates)
	// u och v �r mellan -1 och 1
	float u = (2.0f * i) / width - 1.0f;
	float v = (2.0f * j) / height - 1.0f;

	// Returnerar en normaliserad vektor fr�n kamerans position till u och v
	return glm::normalize(glm::vec3(0.0f, u, v) - location);
}