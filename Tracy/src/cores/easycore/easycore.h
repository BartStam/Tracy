#pragma once

#include "rendercore.h"
#include "shader.h"
#include "camera.h"

struct HitRecord {
	glm::vec3 color;
	float distance;
};

struct Material {
	glm::vec3 color;

	bool isEmissive() const {
		return color.r > 1.0f || color.g > 1.0f || color.b > 1.0f;
	}
};

struct Sphere {
	glm::vec3 center;
	float radius;
	uint32_t materialIndex;
};

struct Triangle {
	glm::vec3 vertex0;
	glm::vec3 vertex1;
	glm::vec3 vertex2;
	glm::vec3 normal;
	uint32_t materialIndex;
};

class EasyCore : public RenderCore {
public:
	EasyCore(uint32_t windowWidth, uint32_t windowHeight);

	void setMaterialData(const std::vector<CoreMaterial>& materials) override;
	void setSphereData(const std::vector<CoreSphere>& spheres) override;
	void setTriangleData(const std::vector<CoreTriangle>& triangles) override;
	void setPointLightData(const std::vector<glm::vec3>& pointLight) override;
	void setSkydomeData(const std::string& path);

	void processInput(GLFWwindow* window) override;
	const std::vector<uint8_t>& nextFrame() override;

private:
	// OpenGL
	Shader m_Shader; // Vertex + fragment shader pair

	// Geometry data
	std::vector<Material> m_MaterialData;
	std::vector<Sphere> m_SphereData;
	std::vector<Triangle> m_TriangleData;

	// Skydome (texture must have exactly 2:1 aspect ratio)
	std::vector<glm::vec3> m_SkydomeData;
	float m_SkydomeRadius = 100.0f;
	int m_SkydomeWidth = 0; // Signed int because that's what is used by stb_image
	int m_SkydomeHeight = 0;

	// Frame
	std::vector<uint8_t> m_Frame;
	const uint32_t m_FrameHeight;
	const uint32_t m_FrameWidth;

	Camera m_Camera;
	uint32_t m_SamplesPerPixel = 4;

	const HitRecord trace(const Ray& ray, uint32_t depth = 1) const;
	const HitRecord traceIS(const Ray& ray, uint32_t depth = 1) const;
};
