#pragma once

struct CoreMaterial {
	glm::vec3 color;
	float diffuse; // Specularity equals (1 - diffuse)
};

struct CoreSphere {
	glm::vec3 center;
	float radius;
	uint32_t materialIndex;
};

struct CoreTriangle {
	glm::vec3 vertex0;
	glm::vec3 vertex1;
	glm::vec3 vertex2;
	glm::vec3 normal;
	uint32_t materialIndex;
};

class RenderCore {
public:
	virtual void setMaterialData(const std::vector<CoreMaterial>& materials) = 0;
	virtual void setSphereData(const std::vector<CoreSphere>& spheres) = 0;
	virtual void setTriangleData(const std::vector<CoreTriangle>& triangles) = 0;
	virtual void setPointLightData(const std::vector<glm::vec3>& pointLight) = 0;

	virtual void processInput(GLFWwindow* window) = 0;
	virtual const std::vector<uint8_t>& nextFrame() = 0;
};