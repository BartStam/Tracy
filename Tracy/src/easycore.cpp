#include "pch.h"

EasyCore::EasyCore(uint32_t windowWidth, uint32_t windowHeight)
	: m_FrameWidth(windowWidth)
	, m_FrameHeight(windowHeight) {
	m_Frame.resize(3 * windowWidth * windowHeight, 0);
	m_Camera = Camera(windowWidth, windowHeight, 120, glm::vec3(-6.0f, 4.0f, 2.0f), glm::vec3(1.0f, -1.0f, 0.0f));
}

void EasyCore::setSphereData(const std::vector<Sphere>& spheres) {
	m_SphereData = spheres;
}

void EasyCore::setPointLightData(const std::vector<glm::vec3>& pointLight) {

}

void EasyCore::setSkydomeData(const std::string& path) {
	int width, height, channels; // Channels is a throwaway value since we force 3 channels per pixel
	
	// Before decoding check if the file is a supported format
	if (stbi_info(path.c_str(), &width, &height, &channels) == 0){
		std::cout << "Skydome texture not found or unsupported format." << std::endl;
		return;
	}

	// And check if the texture has the right aspect ratio
	if (!(width == 2 * height)) {
		std::cout << "Skydome does not have a 2:1 aspect ratio." << std::endl;
		return;
	}

	stbi_set_flip_vertically_on_load(true);
	uint8_t* skydomeData = stbi_load(path.c_str(), &width, &height, &channels, 3);

	// m_SkydomeWidth and m_SkydomeHeight remain unchanged if data failed to load
	if (skydomeData) {
		m_SkydomeWidth = width;
		m_SkydomeHeight = height;

		m_SkydomeData.reserve(m_SkydomeWidth * m_SkydomeHeight);

		// Convert [0, 255] channel values to [0.0f, 1.0f]
		for (int i = 0; i < m_SkydomeWidth * m_SkydomeHeight; i++) {
			glm::vec3 color(skydomeData[3 * i] / 255.0f, skydomeData[3 * i + 1] / 255.0f, skydomeData[3 * i + 2] / 255.0f);
			m_SkydomeData.push_back(color);
		}
	} else {
		std::cout << "Failed to load skydome data." << std::endl;
	}

	stbi_image_free(skydomeData);
}

void EasyCore::processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		m_Camera.tilt(FPI / 180.0f);
	}
	
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		m_Camera.pan(-FPI / 180.0f);
	}
	
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		m_Camera.tilt(-FPI / 180.0f);
	}
	
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		m_Camera.pan(FPI / 180.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		m_Camera.move(glm::vec2(0.0f, 1.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		m_Camera.move(glm::vec2(-1.0f, 0.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		m_Camera.move(glm::vec2(0.0f, -1.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		m_Camera.move(glm::vec2(1.0f, 0.0f));
	}
}

glm::vec3 EasyCore::rayColor(const Ray& ray) const {
	float t;
	float nearestT = FLT_MAX; // Distance to nearest intersection
	glm::vec3 nearestIntersection;
	glm::vec3 nearestNormal;
	glm::vec3 nearestColor;

	// Intersect spheres
	for (const Sphere& sphere : m_SphereData) {
		if (tmath::intersectSphere(ray, sphere.center, sphere.radius, t)) {
			// If the intersection is closer than what was previously found
			if (t < nearestT) {
				nearestT = t;
				nearestIntersection = ray.at(t);
				nearestNormal = glm::normalize(ray.at(t) - sphere.center);
				nearestColor = sphere.color;
			}
		}
	}

	// Intersect floor quad
	if (tmath::intersectTriangle(ray, m_FloorQuad[0], m_FloorQuad[1], m_FloorQuad[3], t)) {
		if (t < nearestT) {
			nearestT = t;
			nearestIntersection = ray.at(t);
			nearestNormal = m_FloorNormal;
			nearestColor = m_FloorColor;
		}
	} else if (tmath::intersectTriangle(ray, m_FloorQuad[1], m_FloorQuad[2], m_FloorQuad[3], t)) { // Ray can't intersect both floor triangles
		if (t < nearestT) {
			nearestT = t;
			nearestIntersection = ray.at(t);
			nearestNormal = m_FloorNormal;
			nearestColor = m_FloorColor;
		}
	}

	// If we intersected any geometry
	if (nearestT < FLT_MAX) {
		// Add some test illumination
		const float illumination = (1.0f - glm::dot(nearestNormal, glm::normalize(glm::vec3(0.5f, -0.5f, 0.2f)))) / 2;
		return nearestColor * illumination;
	}

	// If the ray did not hit any geometry it hits the skybox
	if (m_SkydomeData.size() > 0) {
		const float u = 1 + glm::atan(ray.direction().x, -ray.direction().z) * INVPI;
		const float v = acos(ray.direction().y) * INVPI;

		const size_t width = (size_t)round(u * m_SkydomeWidth);
		const size_t height = (size_t)round(v * m_SkydomeHeight);

		return glm::clamp(m_SkydomeData[glm::min(height * m_SkydomeHeight * 2 + width, m_SkydomeData.size() - 1)], 0.0f, 1.0f);
	}
	
	return glm::vec3(0.04f, 0.03f, 0.08f); // If there is no skybox, return a pretty dark color
}

const std::vector<uint8_t>& EasyCore::nextFrame() {
	for (uint32_t y = 0; y < m_FrameHeight; y++) {
		for (uint32_t x = 0; x < m_FrameWidth; x++) {
			const uint32_t i = 3 * (y * m_FrameWidth + x);

			const float u = (x + 0.5f) / m_FrameWidth;
			const float v = (y + 0.5f) / m_FrameHeight;

			Ray ray = m_Camera.getRay(u, v);
			glm::vec3 color = rayColor(ray);

			m_Frame[i + 0] = static_cast<uint8_t>(glm::clamp(256 * color.r, 0.0f, 255.99f));
			m_Frame[i + 1] = static_cast<uint8_t>(glm::clamp(256 * color.g, 0.0f, 255.99f));
			m_Frame[i + 2] = static_cast<uint8_t>(glm::clamp(256 * color.b, 0.0f, 255.99f));
		}
	}

	return m_Frame;
}
