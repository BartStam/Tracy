#pragma once

class EasyCore : public RenderCore {
public:
	EasyCore(uint32_t windowWidth, uint32_t windowHeight);

	void setSphereData(const std::vector<Sphere>& spheres) override;
	void setTriangleData(const std::vector<Triangle>& triangles) override;
	void setPointLightData(const std::vector<glm::vec3>& pointLight) override;
	void setSkydomeData(const std::string& path);

	void processInput(GLFWwindow* window) override;
	const std::vector<uint8_t>& nextFrame() override;

private:
	// Geometry data
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

	const HitRecord trace(const Ray& ray, uint32_t depth = 1) const;
};
