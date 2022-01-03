#pragma once

class EasyCore : public RenderCore {
public:
	EasyCore(uint32_t windowWidth, uint32_t windowHeight);

	void setSphereData(const std::vector<Sphere>& spheres) override;
	void setPointLightData(const std::vector<glm::vec3>& pointLight) override;
	void setSkydomeData(const std::string& path);

	const std::vector<uint8_t>& nextFrame() override;

private:
	// Floor quad data
	glm::vec3 m_FloorColor = glm::vec3(0.5f, 0.5f, 0.6f);
	glm::vec3 m_FloorNormal = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 m_FloorQuad[4] = {
		glm::vec3( 5.0f, 0.0f,  5.0f),
		glm::vec3( 5.0f, 0.0f, -5.0f),
		glm::vec3(-5.0f, 0.0f, -5.0f),
		glm::vec3(-5.0f, 0.0f,  5.0f)
	};

	// Geometry data (only spheres "for now")
	std::vector<Sphere> m_SphereData;

	// Skydome (texture must have exactly 2:1 aspect ratio)
	std::vector<glm::vec3> m_SkydomeData;
	int m_SkydomeWidth = 0;
	int m_SkydomeHeight = 0;

	// Frame
	std::vector<uint8_t> m_Frame;
	const uint32_t m_FrameHeight;
	const uint32_t m_FrameWidth;

	Camera m_Camera;

	glm::vec3 rayColor(const Ray& ray) const;
};
