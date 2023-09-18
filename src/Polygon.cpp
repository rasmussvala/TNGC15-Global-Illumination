#include "../include/Polygon.h"

Rectangle::Rectangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4)
	: vertex1(p1), vertex2(p2), vertex3(p3), vertex4(p4) {}

bool Rectangle::intersect(const Ray& ray, glm::vec3& intersectionPoint) const {

	// Implementera senare
	return false;
}

Triangle::Triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	: vertex1(p1), vertex2(p2), vertex3(p3) {}

bool Triangle::intersect(const Ray& ray, glm::vec3& intersectionPoint) const {
    // M�ller�Trumbore algoritm f�r att hitta snitt mellan ray och triangel

    const float EPSILON = 1e-6;

    glm::vec3 edge1 = vertex2 - vertex1;
    glm::vec3 edge2 = vertex3 - vertex1;
    glm::vec3 h = glm::cross(ray.getDirection(), edge2);
    float a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false; // Ray �r parallell med triangeln

    float f = 1.0f / a;
    glm::vec3 s = ray.getOrigin() - vertex1;
    float u = f * glm::dot(s, h);

    if (u < 0.0 || u > 1.0)
        return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(ray.getDirection(), q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    // Vi har ett snitt, ber�kna t f�r att hitta snittpunkten
    float t = f * glm::dot(edge2, q);

    if (t > EPSILON) {
        intersectionPoint = ray.at(t);
        return true;
    }

    return false; // Snittet �r bakom ray origin (t < 0)
}

