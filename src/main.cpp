#pragma once

#include "../include/Scene.h"
#include "../include/Polygon.h"
#include "../include/Cube.h"
#include "../include/Sphere.h"

int main() {
	// max 180 x 180 atm
	Scene myScene(180, 180);

	/*Cube cube1{};
	myScene.addCube(&cube1);*/

	// Settings for Sphere 
	glm::vec3 center{ 8.0f, 2.5f, -3.5f };
	float radius = 1.5f;
	ColorRGB blue{ 0.0f,0.0f,0.4f };
	Material materialBlue{ REFLECTIVE };

	Sphere sphere1{center, radius, materialBlue};
	myScene.addGeometry(&sphere1);

	Light light1{ glm::vec3{5.0f, 1.0f, 4.9999f},
		glm::vec3{3.0f, 1.0f, 4.9999f},
		glm::vec3{3.0f, -1.0f, 4.9999f},
		glm::vec3{5.0f, -1.0f, 4.9999f}
	};

	myScene.addLight(&light1);

	int depth = 3; // > 1
	int nrOfShadowRays = 10; // > 1
	int nrOfIndirectRays = 4;

	myScene.render(depth, nrOfShadowRays, nrOfIndirectRays);

	return 0;
}

// @TODO - kontrollera ny direction f�r randomRays
// @TODO - memoryleaks! 
// @TODO - fixa rendering till glas
// @TODO - fixa s� att man ser ljusk�llan (fixa senare)
// @TODO - fixa antialiasing
// @TODO - t�nk p� att ytan v�nda fel kastas bort i traingle::intersect (hur glas ska hantera det) om bool insideObj = true, ignorera? 