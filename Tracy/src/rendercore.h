#pragma once

struct Sphere {
	glm::vec3 center;
	float radius;
	glm::vec3 color;
};

class RenderCore {
public:
	virtual void setSphereData(const std::vector<Sphere>& spheres) = 0;
	virtual void setPointLightData(const std::vector<glm::vec3>& pointLight) = 0;

	virtual void processInput(GLFWwindow* window) = 0;
	virtual const std::vector<uint8_t>& nextFrame() = 0;
};