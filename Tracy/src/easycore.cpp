#include "pch.h"
#include "easycore.h"
#include "math/tracymath.h"

EasyCore::EasyCore(uint32_t windowWidth, uint32_t windowHeight)
	: m_FrameWidth(windowWidth)
	, m_FrameHeight(windowHeight) {
	uint32_t size = 3 * windowWidth * windowHeight;
	m_Frame.resize(size, 0);

	m_Camera = Camera(windowWidth, windowHeight, 90, glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	std::cout << "Lower left corner: " << glm::to_string(m_Camera.lowerLeftCorner()) << std::endl;
	std::cout << "Horizontal: " << glm::to_string(m_Camera.horizontal()) << std::endl;
	std::cout << "Vertical: " << glm::to_string(m_Camera.vertical()) << std::endl;
}

void EasyCore::setSphereData(const std::vector<Sphere>& spheres) {
	m_SphereData = spheres;
}

void EasyCore::setPointLightData(const std::vector<glm::vec3>& pointLight) {

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
		float illumination = (1.0f - glm::dot(nearestNormal, glm::normalize(glm::vec3(0.5f, -0.5f, 0.2f)))) / 2;
		return nearestColor * illumination;
	}

	return glm::vec3(0.0f); // Return black if we did not intersect any geometry
}

const std::vector<uint8_t>& EasyCore::nextFrame() {
	for (uint32_t y = 0; y < m_FrameHeight; y++) {
		for (uint32_t x = 0; x < m_FrameWidth; x++) {
			uint32_t i = 3 * (y * m_FrameWidth + x);

			float u = (x + 0.5f) / m_FrameWidth;
			float v = (y + 0.5f) / m_FrameHeight;

			Ray ray = m_Camera.getRay(u, v);
			glm::vec3 color = rayColor(ray);

			m_Frame[i + 0] = static_cast<uint8_t>(glm::clamp(256 * color.r, 0.0f, 255.99f));
			m_Frame[i + 1] = static_cast<uint8_t>(glm::clamp(256 * color.g, 0.0f, 255.99f));
			m_Frame[i + 2] = static_cast<uint8_t>(glm::clamp(256 * color.b, 0.0f, 255.99f));
		}
	}

	return m_Frame;
}
