#pragma once

#include "../include/Camera.h"

Camera::Camera(int w, int h) : width(w), height(h) {
  // Constants will be updated in Scene::render
  MAX_DEPTH_DIFFUSE = 0;
  MAX_DEPTH_REFLECTIVE = 0;
  MAX_SHADOWRAYS = 0;
  MAX_INDIRECTRAYS = 0;

  // Allocate memory for all the pixels
  pixels.resize(height);
  for (int i = 0; i < height; ++i) {
    pixels[i].resize(width);
  }
}

inline static float linear_to_gamma(double linear_component) {
  if (linear_component > 0.0f) return sqrt(linear_component);

  return 0.0f;
}

void Camera::saveImage(std::string filename) {
  std::ofstream ppmFile(filename);  // Opens/creates the file

  ppmFile << "P3\n" << width << ' ' << height << "\n255\n";

  // Write the clamped pixel values to the output file
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      auto r = pixels[j][i].r;
      auto g = pixels[j][i].g;
      auto b = pixels[j][i].b;

      r = linear_to_gamma(r);
      g = linear_to_gamma(g);
      b = linear_to_gamma(b);

      // Convert to 0-255 and clamp
      int ir = std::min((int)(255 * r), 255);
      int ig = std::min((int)(255 * g), 255);
      int ib = std::min((int)(255 * b), 255);

      ppmFile << ir << ' ' << ig << ' ' << ib << '\n';
    }
  }
  ppmFile.close();

  std::cout << "The file has been created at: " + filename + "\n";
}

void Camera::castRays(int raysPerPixel) {
  float progress = 0.0f;
  Ray ray{};

  // Loop through all pixels and assign colors
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      glm::vec3 colorSum(0.0f);

      // Calculate ray direction once per pixel
      glm::vec3 direction = rayDirectionFromCamera(i, j);

      for (int k = 0; k < raysPerPixel; ++k) {
        // Reuse ray object
        ray.setRay(location, direction);

        // Check intersections that occur in the scene
        colorSum += castRay(ray, MAX_DEPTH_DIFFUSE, MAX_DEPTH_REFLECTIVE);
      }

      // Average the color over the number of rays per pixel
      pixels[j][i] = colorSum / static_cast<float>(raysPerPixel);

      // Show progress during rendering
      progressBar(progress / (height * width));
      progress += 1.0f;
    }
  }
}

glm::vec3 Camera::castRay(const Ray& ray, int diffuseBounceCount,
                          int mirrorBounceCount) {
  glm::vec3 color{0.0f, 0.0f, 0.0f};

  // End of recursion, return black
  if (diffuseBounceCount <= 0 || mirrorBounceCount <= 0) {
    return color;
  }

  hitResult hit = closestIntersect(ray, geometries);

  // We have a hit
  if (hit.t > EPSILON && hit.t < FLT_MAX) {
    glm::vec3 hitPoint = ray.at(hit.t);
    Geometry* hitGeometry = geometries[hit.index];

    // REFLECTIVE
    if (hitGeometry->getMaterial().type == REFLECTIVE) {
      glm::vec3 reflectDir =
          glm::reflect(ray.getDirection(), hitGeometry->getNormal(hitPoint));
      Ray reflectedRay(hitPoint, reflectDir);

      color = castRay(reflectedRay, diffuseBounceCount, mirrorBounceCount - 1);
    }
    // DIFFUSE
    else if (hitGeometry->getMaterial().type == DIFFUSE) {
      glm::vec3 direct =
          directLight(hitPoint, hitGeometry->getNormal(hitPoint), hit.index);
      glm::vec3 indirect =
          indirectLight(diffuseBounceCount, mirrorBounceCount, hitPoint,
                        hitGeometry->getNormal(hitPoint));

      color += direct + (indirect * hitGeometry->getMaterial().color);
    }
    // TRANSPARENT
    else {
      // Reflection
      glm::vec3 normal = hitGeometry->getNormal(hitPoint);
      glm::vec3 reflectDir = glm::reflect(ray.getDirection(), normal);
      Ray reflectedRay(hitPoint, reflectDir);
      glm::vec3 reflectColor =
          castRay(reflectedRay, diffuseBounceCount, mirrorBounceCount - 1);

      // Refraction
      float n1 = 1.0f;                                        // Air
      float n2 = hitGeometry->getMaterial().refractiveIndex;  // Glass

      glm::vec3 d0 =
          glm::normalize(ray.getDirection());  // Normalize incident direction
      float cosOmega = glm::clamp(glm::dot(d0, normal), -1.0f, 1.0f);

      // We are inside of the glass
      if (cosOmega > 0) {
        std::swap(n1, n2);
        normal = -normal;  // Invert normal if we are inside the object
      }

      float R = n1 / n2;  // Ratio of refractive indices
      float k =
          1 - R * R * (1 - cosOmega * cosOmega);  // Snell's law discriminant

      glm::vec3 refractColor(0.0f);
      if (k >= 0) {  // Total internal reflection check
        glm::vec3 refractDir =
            R * d0 + (R * cosOmega - sqrtf(k)) *
                         normal;  // Calculate refraction direction
        Ray refractedRay(hitPoint, refractDir);
        refractColor =
            castRay(refractedRay, diffuseBounceCount, mirrorBounceCount - 1);
      }

      // Fresnel effect (using Schlick's approximation)
      float R0 = powf((n1 - n2) / (n1 + n2), 2);
      float fresnel = R0 + (1 - R0) * powf(1.0f - fabs(cosOmega), 5);

      // Combine reflection and refraction based on Fresnel effect
      color = fresnel * reflectColor + (1 - fresnel) * refractColor;
    }
  }

  return color;
}

glm::vec3 Camera::directLight(const glm::vec3& hitPoint,
                              const glm::vec3& hitPointNormal, int index) {
  glm::vec3 colorOfObject = geometries[index]->getMaterial().color;
  float irradiance = 0.0f;

  for (Light* light : lights) {
    irradiance += light->calculateLight(geometries, hitPoint, hitPointNormal,
                                        MAX_SHADOWRAYS);
  }

  return colorOfObject * irradiance;
}

glm::vec3 Camera::indirectLight(int depthDiffuse, int depthReflective,
                                const glm::vec3& hitPoint,
                                const glm::vec3& hitPointNormal) {
  glm::vec3 indirect{0.0f};
  Ray randomRay{};

  for (int i = 0; i < MAX_INDIRECTRAYS; ++i) {
    randomRay.setRay(hitPoint, randomRayDirection(hitPointNormal));
    indirect += castRay(randomRay, depthDiffuse - 1, depthReflective);
  }

  indirect /= static_cast<float>(MAX_INDIRECTRAYS);
  return indirect;
}

glm::vec3 Camera::rayDirectionFromCamera(int i, int j) const {
  // Beräknar u och v (positionen i world coordinates)
  // u och v är mellan -1 och 1
  float u = 1.0f - (2.0f * i) / width;
  float v = 1.0f - (2.0f * j) / height;

  // Returnerar en normaliserad vektor från kamerans position till u och v
  return glm::normalize(glm::vec3(0.0f, u, v) - location);
}

glm::vec3 Camera::randomRayDirection(const glm::vec3& hitPointNormal) {
  float random1 = (float)rand() / RAND_MAX;  // random value [0,1]
  float random2 = (float)rand() / RAND_MAX;  // random value [0,1]
  float phi = 2.0f * PI * random1;           // azimuth [0, 2PI]
  float omega = PI * random2;                // inclination [0, PI]

  glm::vec3 worldDir = sphericalToCartesian(phi, omega, hitPointNormal);

  // Make sure the direction is not pointing back into the surface
  if (glm::dot(worldDir, hitPointNormal) < 0.0f) worldDir = -worldDir;

  return glm::normalize(worldDir);
}

void Camera::progressBar(float percent) {
  const int BAR_WIDTH = 20;

  std::cout << "[";
  int pos = (int)(BAR_WIDTH * percent);
  for (int i = 0; i < BAR_WIDTH; ++i) {
    if (i <= pos)
      std::cout << "=";
    else
      std::cout << " ";
  }
  std::cout << "] " << int(percent * 100.0f + 0.5f)
            << " %\r";  // +0.5 for round-off error
  std::cout.flush();
}

glm::vec3 Camera::sphericalToCartesian(float phi, float omega,
                                       const glm::vec3& normal) {
  glm::vec3 cartesianDir(cos(phi) * sin(omega), sin(phi) * sin(omega),
                         cos(omega));

  // Old version, didn't look good
  /*
  glm::vec3 c = normal;
  glm::vec3 a = glm::normalize(-cartesianDir + glm::dot(normal, cartesianDir) *
  normal); glm::vec3 b = glm::cross(c, a); glm::mat3 transformationMatrix(a, b,
  c); return transformationMatrix * cartesianDir;
  */
  return cartesianDir;
}

void Camera::configure(const std::vector<Geometry*>& newGeometries,
                       const std::vector<Light*>& newLights,
                       int newDepthDiffuse, int newDepthReflective,
                       int newShadowRays, int newIndirectRays) {
  geometries = newGeometries;
  lights = newLights;
  MAX_DEPTH_DIFFUSE = newDepthDiffuse;
  MAX_DEPTH_REFLECTIVE = newDepthReflective;
  MAX_SHADOWRAYS = newShadowRays;
  MAX_INDIRECTRAYS = newIndirectRays;
}

hitResult closestIntersect(const Ray& ray,
                           const std::vector<Geometry*> geometries) {
  const float EPSILON = 1e-4f;

  float closestT = FLT_MAX;
  int closestIndex = -1;  // Initialize to an invalid index

  // Check intersections with polygons
  for (int i = 0; i < geometries.size(); i++) {
    float t = geometries[i]->intersect(ray);
    if (t > EPSILON && t < closestT) {
      closestT = t;
      closestIndex = i;
    }
  }

  return {closestT, closestIndex};
}
