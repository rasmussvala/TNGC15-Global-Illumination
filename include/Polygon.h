#pragma once

#include <algorithm>

#include "Geometry.h"

class Triangle : public Geometry {
 public:
  Triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
           const Material& material);

  float intersect(const Ray& ray) const override;
  glm::vec3 getNormal(const glm::vec3& point) const override;
  Material getMaterial() const override;

 private:
  glm::vec3 v1, v2, v3;
  Material material;
  glm::vec3 normal;
};

class Rectangle : public Geometry {
 public:
  Rectangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
            const glm::vec3& p4, const Material& material);
  Rectangle(const Rectangle& other);

  float intersect(const Ray& ray) const override;
  glm::vec3 getNormal(const glm::vec3& point) const override;
  Material getMaterial() const override;

 private:
  glm::vec3 v1, v2, v3, v4;
  Material material;
  glm::vec3 normal;
};
