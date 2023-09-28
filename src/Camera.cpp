#include "../include/Camera.h"
#include "../include/Light.h"
#include "../include/Object.h"
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

void Camera::saveImage(std::string filename) {

	// NO CONTRAST STRETCHING

	//std::ofstream ppmFile(filename); // �ppnar/skapar filen

	//ppmFile << "P3\n" << width << ' ' << height << "\n255\n";

	//for (int j = 0; j < height; ++j) {
	//	for (int i = 0; i < width; ++i) {

	//		auto r = pixels[j][i].r;
	//		auto g = pixels[j][i].g;
	//		auto b = pixels[j][i].b;

	//		// Convert from floats [0,1] to ints [0,255]
	//		int ir = static_cast<int>(255.999 * r);
	//		int ig = static_cast<int>(255.999 * g);
	//		int ib = static_cast<int>(255.999 * b);

	//		ppmFile << ir << ' ' << ig << ' ' << ib << '\n';
	//	}
	//}

	//ppmFile.close();

	// CONTRAST STRETCHING

	std::ofstream ppmFile(filename); // Öppnar/skapar filen

	ppmFile << "P3\n" << width << ' ' << height << "\n255\n";

	// Hittar den starkaste färgen i bilden
	float maxRGB = -INFINITY;

	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			auto r = pixels[j][i].r;
			auto g = pixels[j][i].g;
			auto b = pixels[j][i].b;

			float maxChannel = std::max({ r, g, b });
			maxRGB = std::max(maxRGB, maxChannel);
		}
	}

	// Beräknar faktorn som ska användas för att stretcha färgerna
	float stretchFactor = 255.0 / maxRGB;

	// Skriver ut färgerna till filen
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			auto r = pixels[j][i].r;
			auto g = pixels[j][i].g;
			auto b = pixels[j][i].b;

			// Applicerar faktorn på färgerna
			int ir = static_cast<int>(stretchFactor * r);
			int ig = static_cast<int>(stretchFactor * g);
			int ib = static_cast<int>(stretchFactor * b);

			ppmFile << ir << ' ' << ig << ' ' << ib << '\n';
		}
	}

	ppmFile.close();


}

void Camera::traceRays(const std::vector<Polygon*>& polygons, const std::vector<Light*>& ligths, const std::vector<Object*>& objects) {
	// Loopar igenom alla pixlar
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {

			// Kollar om ray intersectar något objekt
			renderRoom(polygons, ligths, objects, j, i);

			const float EPSILON = 1e-6;

			for (const auto& obj : objects) {
				float t = obj->intersect(Ray(location, calculateRayDirection(i, j)));

				if (t > EPSILON) {
					glm::vec3 intersectionPoint = Ray(location, calculateRayDirection(i, j)).at(t);
					/*glm::vec3 intersectionPointNormal = obj->getNormal(intersectionPoint);
					float irradiance = 0.0f;

					for (Light* light : ligths) {
						irradiance = light->calculateLight(intersectionPoint, intersectionPointNormal, objects);
					}*/

					// Ansätter färgen på pixeln 
					pixels[j][i] = { obj->getColor().r,
						obj->getColor().g,
						obj->getColor().b
					};
				}
			}
		}
	}
}

void Camera::renderRoom(const std::vector<Polygon*>& polygons, const std::vector<Light*>& lights, const std::vector<Object*>& objects, int j, int i) {

	// Skapar en ray för varje pixel
	Ray ray(location, calculateRayDirection(i, j));

	// Kollar om ray intersectar något objekt
	for (const Polygon* obj : polygons) {

		const float EPSILON = 1e-6;

		float t = obj->intersect(ray);

		if (t > EPSILON) {

			glm::vec3 intersectionPoint = ray.at(t);
			glm::vec3 intersectionPointNormal = obj->getNormal();
			float irradiance = 0.0f;

			for (Light* light : lights) {
				irradiance = light->calculateLight(intersectionPoint, intersectionPointNormal, objects);
			}

			// Ansätter färgen på pixeln 
			pixels[j][i] = { obj->getColor().r * irradiance,
				obj->getColor().g * irradiance,
				obj->getColor().b * irradiance };
		}
	}
}

glm::vec3 Camera::calculateRayDirection(int i, int j) {
	// Ber�knar u och v (positionen i world coordinates)
	// u och v är mellan -1 och 1
	float u = (2.0f * i) / width - 1.0f; // går från -1 -> 1
	float v = 1.0f - (2.0f * j) / height; // går från 1 -> -1

	// Returnerar en normaliserad vektor fr�n kamerans position till u och v
	return glm::normalize(glm::vec3(0.0f, u, v) - location);
}