#pragma once

#include "../include/Cube.h"
#include "../include/Polygon.h"
#include "../include/Scene.h"
#include "../include/Sphere.h"

int main()
{
  // 400x400 pixels is max on my pc with low settings
  Scene myScene(200, 200);

  // Default cube adds a blue cube to right hand side
  Cube cube1{};
  myScene.addCube(&cube1);

  // Create a sphere
  glm::vec3 center{5.5f, 2.5f, -3.5f};
  float radius = 1.5f;
  Material material{TRANSPARENT};
  Sphere sphere{center, radius, material};
  myScene.addGeometry(&sphere);

  glm::vec3 center2{8.8f, 2.5f, -3.5f};
  float radius2 = 1.5f;
  glm::vec3 color2{0.4f, 0.4f, 0.4f};
  Material material2{DIFFUSE, color2};
  Sphere sphere2{center2, radius2, material2};
  myScene.addGeometry(&sphere2);

  // Create a rectangular light with a position just below the roof
  Light light{glm::vec3{5.0f, 1.0f, 4.9999f}, glm::vec3{5.0f, -1.0f, 4.9999f},
              glm::vec3{3.0f, -1.0f, 4.9999f}, glm::vec3{3.0f, 1.0f, 4.9999f}};
  myScene.addLight(&light);

  // ----- Parameters to change -----

  // Determines the number of bounces on diffuse objects, recommends 2
  int diffuseBounceCount = 2;

  // Determines the number of bounces on mirrors, recommends 5 for
  // mirror-in-mirror effect
  int mirrorBounceCount = 10;

  // Determines the number of shadow rays to shoot at each light source in the
  // scene. Mark recommends 100, but that's demanding
  int shadowRayCount = 10;

  // Determines the number of rays to create when hitting a diffuse object
  int indirectRayCount = 5;

  // Determines the number of rays shot from a single pixel, then averages them
  // Demanding!
  int raysPerPixel = 4;

  myScene.render(diffuseBounceCount, mirrorBounceCount, shadowRayCount,
                 indirectRayCount, raysPerPixel, "../images");

  return 0;
}

// TODO 1: Fix rendering for glass
// TODO 2: Check how colors blend - direct light and indirect light
// TODO 3: Make sure the light source is visible
// TODO 4: Multicore, implemented in ray casting but I think I need to make more
// changes
