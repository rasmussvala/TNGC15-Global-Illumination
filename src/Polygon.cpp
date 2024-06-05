#include "../include/Polygon.h"

float Triangle::intersect(const Ray& ray) const {
  const float EPSILON = 1e-4f;
  glm::vec3 D = ray.getDirection();

  // Vi träffar fel sida av triangeln eller är parallell med ytan
  /*if (glm::dot(D, normal) > -EPSILON) {
    return -1.0f;
  }*/

  glm::vec3 E1 = v2 - v1;
  glm::vec3 E2 = v3 - v1;
  glm::vec3 T = ray.getOrigin() - v1;
  glm::vec3 P = glm::cross(D, E2);
  glm::vec3 Q = glm::cross(T, E1);

  float t = (glm::dot(Q, E2) / glm::dot(P, E1));
  float u = (glm::dot(P, T) / glm::dot(P, E1));
  float v = (glm::dot(Q, D) / glm::dot(P, E1));

  // Vi träffar utanför trianeln eller är alldeles för nära (t < EPSILON)
  if (u + v > 1.0f + EPSILON || u < -EPSILON || v < -EPSILON || t < EPSILON) {
    return -1.0f;
  }

  return t;
}

Triangle::Triangle(const glm::vec3& p1, const glm::vec3& p2,
                   const glm::vec3& p3, const Material& mat)
    : v1(p1), v2(p2), v3(p3), material(mat) {
  normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
}

glm::vec3 Triangle::getNormal(const glm::vec3& /* point */) const {
  return normal;
}

Material Triangle::getMaterial() const { return material; }

Rectangle::Rectangle(const glm::vec3& p1, const glm::vec3& p2,
                     const glm::vec3& p3, const glm::vec3& p4,
                     const Material& mat)
    : v1(p1), v2(p2), v3(p3), v4(p4), material(mat) {
  normal = glm::normalize(glm::cross(v2 - v1, v4 - v1));
}

Rectangle::Rectangle(const Rectangle& other)
    : v1(other.v1),
      v2(other.v2),
      v3(other.v3),
      v4(other.v4),
      material(other.material),
      normal(other.normal) {}

float Rectangle::intersect(const Ray& ray) const {
  Triangle* triangle1 = new Triangle(v1, v2, v3, material);
  Triangle* triangle2 = new Triangle(v1, v3, v4, material);

  // Kollar snittet mellan ray och de två trianglarna som utgör rektangeln
  float t1 = triangle1->intersect(ray);
  float t2 = triangle2->intersect(ray);

  delete triangle1;
  delete triangle2;

  const float EPSILON = 1e-4f;

  // Kollar om snittet är större än EPSILON
  if (t1 >= EPSILON && t2 >= EPSILON) {
    return std::min(t1, t2);  // Returnerar det minsta snittet

  } else if (t1 >= EPSILON) {
    return t1;
  } else if (t2 >= EPSILON) {
    return t2;
  } else {
    return -1.0f;  // Inget snitt
  }
}

glm::vec3 Rectangle::getNormal(const glm::vec3& /* point */) const {
  return normal;
}

Material Rectangle::getMaterial() const { return material; }
