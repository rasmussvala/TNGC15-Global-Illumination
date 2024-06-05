#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Cube.h"
#include "Light.h"
#include "Polygon.h"
#include "Sphere.h"
#include "glm/glm.hpp"

struct hitResult {
  float t;    // At which length (t) we get a hit
  int index;  // Index of the closest intersection with all geometries
};

hitResult closestIntersect(const Ray& ray,
                           const std::vector<Geometry*> geometries);

class Camera {
 public:
  Camera(int w, int h);

  // Save the image as a .ppm file to the specified
  void saveImage(std::string filename);

  // Loop through all the pixels created for the cameras viewports and cast rays
  void castRays(int samplesPerPixel);

  // Cast a ray and return the color of that ray
  glm::vec3 castRay(const Ray& ray, int depthDiffuse, int depthReflective);

  // Return the color of the direct light
  glm::vec3 directLight(const glm::vec3& hitPoint,
                        const glm::vec3& hitPointNormal, int index);

  // Return the color of the indirect light
  glm::vec3 indirectLight(int depthDiffuse, int depthReflective,
                          const glm::vec3& hitPoint,
                          const glm::vec3& hitPointNormal);

  // Calculate the ray direction from the cameras origin to it's viewport
  glm::vec3 rayDirectionFromCamera(int i, int j) const;

  // Calulate a new random ray direction based on the intersection
  glm::vec3 randomRayDirection(const glm::vec3& hitPointNormal);

  // Print the progress of the render in the console
  void progressBar(float percent);

  // Convert from hemispherical coordinate to world coordinate
  glm::vec3 sphericalToCartesian(float phi, float omega,
                                 const glm::vec3& normal);

  // Adds geometry, light, and configures render settings
  void configure(const std::vector<Geometry*>& newGeometries,
                 const std::vector<Light*>& newLights, int newDepthDiffuse,
                 int newDepthReflective, int newShadowRays,
                 int newIndirectRays);

 private:
  int width;
  int height;
  glm::vec3 location{-1.0f, 0.0f, 0.0f};

  std::vector<Geometry*> geometries;
  std::vector<Light*> lights;

  int MAX_DEPTH_DIFFUSE;
  int MAX_DEPTH_REFLECTIVE;
  int MAX_SHADOWRAYS;
  int MAX_INDIRECTRAYS;

  std::vector<std::vector<glm::vec3>> pixels;

  const float EPSILON = 1e-4f;
  const float PI = 3.14159265f;
};
