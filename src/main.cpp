#pragma once

#include "../include/Cube.h"
#include "../include/Polygon.h"
#include "../include/Scene.h"
#include "../include/Sphere.h"

int main() {
  // Set resolution of the rendering
  Scene myScene(600, 600);

  // Default cube adds a white cube to right hand side
  Cube cube1{};
  myScene.addCube(&cube1);

  // Create a sphere
  glm::vec3 center{3.4f, 0.0f, 0.0f};
  float radius = 1.5f;
  Material material{TRANSPARENT, 1.5f};
  Sphere sphere{center, radius, material};
  myScene.addGeometry(&sphere);

  // Create another sphere
  glm::vec3 center2{6.5f, 2.5f, -3.5f};
  float radius2 = 1.5f;
  Material material2{REFLECTIVE};
  Sphere sphere2{center2, radius2, material2};
  myScene.addGeometry(&sphere2);

  // Create a rectangular light with a position just below the roof
  Light light{glm::vec3{6.0f, 1.0f, 4.9999f}, glm::vec3{6.0f, -1.0f, 4.9999f},
              glm::vec3{4.0f, -1.0f, 4.9999f}, glm::vec3{4.0f, 1.0f, 4.9999f}};
  myScene.addLight(&light);

  // ----- Parameters to change -----

  // Determines the number of bounces on diffuse objects, recommends 2
  int diffuseBounceCount = 2;

  // Determines the number of bounces on mirrors, recommends 5 for
  // mirror-in-mirror effect
  int mirrorBounceCount = 10;

  // Determines the number of shadow rays to shoot at each light source in the
  // scene. Mark recommends 100, but that's demanding
  int shadowRayCount = 50;

  // Determines the number of rays to create when hitting a diffuse object
  int indirectRayCount = 10;

  // Determines the number of rays shot from a single pixel, then averages them
  // Demanding!
  int raysPerPixel = 5;

  myScene.render(diffuseBounceCount, mirrorBounceCount, shadowRayCount,
                 indirectRayCount, raysPerPixel, "../images");

  return 0;
}
