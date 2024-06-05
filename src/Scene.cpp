#include "../include/Scene.h"

#include "../include/Camera.h"
#include "../include/Polygon.h"

Scene::Scene(int width, int height) : camera(width, height) {
  // Färger
  glm::vec3 white(0.4, 0.4, 0.4);
  glm::vec3 blue(0.0, 0.0, 0.4);
  glm::vec3 red(0.4, 0.0, 0.0);
  glm::vec3 green(0.0, 0.4, 0.0);

  Material materialWhite(DIFFUSE, white);
  Material materialBlue(DIFFUSE, blue);
  Material materialRed(DIFFUSE, red);
  Material materialGreen(DIFFUSE, green);
  Material mirror(REFLECTIVE);
  // Material glas(TRANSPARENT, 1.5);  // glas with refractiveIndex 1.5

  // Golv
  glm::vec3 p0(0.0f, 6.0f, -5.0f);
  glm::vec3 p1(10.0f, 6.0f, -5.0f);
  glm::vec3 p2(13.0f, 0.0f, -5.0f);
  glm::vec3 p3(10.0f, -6.0f, -5.0f);
  glm::vec3 p4(0.0f, -6.0f, -5.0f);
  glm::vec3 p5(-3.0f, 0.0f, -5.0f);

  // Tak
  glm::vec3 p6(0.0f, 6.0f, 5.0f);
  glm::vec3 p7(10.0f, 6.0f, 5.0f);
  glm::vec3 p8(13.0f, 0.0f, 5.0f);
  glm::vec3 p9(10.0f, -6.0f, 5.0f);
  glm::vec3 p10(0.0f, -6.0f, 5.0f);
  glm::vec3 p11(-3.0f, 0.0f, 5.0f);

  // Glov
  Triangle* triangle_floorB = new Triangle(p0, p5, p4, materialWhite);
  Triangle* triangle_floorF = new Triangle(p1, p3, p2, materialWhite);
  Rectangle* rectangle_floor = new Rectangle(p0, p4, p3, p1, materialWhite);

  // Väggar
  Rectangle* rectangle_wallH = new Rectangle(p3, p4, p10, p9, materialRed);
  Rectangle* rectangle_wallHB = new Rectangle(p4, p5, p11, p10, materialWhite);
  Rectangle* rectangle_wallHF = new Rectangle(p2, p3, p9, p8, mirror);
  Rectangle* rectangle_wallV = new Rectangle(p0, p1, p7, p6, materialGreen);
  Rectangle* rectangle_wallVB = new Rectangle(p0, p6, p11, p5, materialWhite);
  Rectangle* rectangle_wallVF = new Rectangle(p1, p2, p8, p7, materialWhite);

  // Tak
  Triangle* triangle_ceilingB = new Triangle(p6, p10, p11, materialWhite);
  Triangle* triangle_ceilingF = new Triangle(p7, p8, p9, materialWhite);
  Rectangle* rectangle_ceiling = new Rectangle(p6, p7, p9, p10, materialWhite);

  // Lägger till alla polygoner i scenen
  addGeometry(triangle_floorB);
  addGeometry(triangle_floorF);
  addGeometry(rectangle_floor);
  addGeometry(rectangle_wallH);
  addGeometry(rectangle_wallHB);
  addGeometry(rectangle_wallHF);
  addGeometry(rectangle_wallV);
  addGeometry(rectangle_wallVB);
  addGeometry(rectangle_wallVF);
  addGeometry(triangle_ceilingB);
  addGeometry(triangle_ceilingF);
  addGeometry(rectangle_ceiling);
}

void Scene::addLight(Light* light) {
  lights.push_back(light);

  Rectangle* lightGeometry = new Rectangle{light->getGeometry()};
  addGeometry(lightGeometry);
}

void Scene::addGeometry(Geometry* geometry) { geometries.push_back(geometry); }

void Scene::addCube(Cube* cube) {
  std::vector<Geometry*> surfaces = cube->getSurfaces();
  for (Geometry* surface : surfaces) {
    geometries.push_back(surface);
  }
}

void Scene::render(int diffuseBounceCount, int mirrorBounceCount,
                   int shadowRayCount, int indirectRayCount, int raysPerPixel,
                   std::string outputPath) {
  // Adds geometry, light, and configures render settings
  camera.configure(geometries, lights, diffuseBounceCount, mirrorBounceCount,
                   shadowRayCount, indirectRayCount);

  camera.castRays(raysPerPixel);
  camera.saveImage(outputPath + "/output.ppm");
}
