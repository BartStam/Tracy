#pragma once

struct Material {
	glm::vec3 color;
	float diffuse; // Specularity equals (1 - diffuse)

	bool isEmissive() const {
		return color.r > 1.0f || color.g > 1.0f || color.b > 1.0f;
	}
};

struct HitRecord {
	glm::vec3 color;
	float distance;
};

struct Sphere {
	glm::vec3 center;
	float radius;
	Material material;
};

struct Triangle {
	glm::vec3 vertex0;
	glm::vec3 vertex1;
	glm::vec3 vertex2;
	glm::vec3 normal;
	Material material;
};

class RenderCore {
public:
	virtual void setSphereData(const std::vector<Sphere>& spheres) = 0;
	virtual void setTriangleData(const std::vector<Triangle>& triangles) = 0;
	virtual void setPointLightData(const std::vector<glm::vec3>& pointLight) = 0;

	virtual void processInput(GLFWwindow* window) = 0;
	virtual const std::vector<uint8_t>& nextFrame() = 0;
};