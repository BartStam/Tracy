#include "pch.h"
#include "easycore.h"

EasyCore::EasyCore(uint32_t windowWidth, uint32_t windowHeight)
	: m_FrameWidth(windowWidth)
	, m_FrameHeight(windowHeight) {
	m_Frame.resize(3 * windowWidth * windowHeight, 0);
	m_Camera = Camera(windowWidth, windowHeight, 120, glm::vec3(-6.0f, 4.0f, 2.0f), glm::vec3(1.0f, -1.0f, 0.0f));
	m_Shader = Shader("src/cores/easycore/vertex_shader.glsl", "src/cores/easycore/fragment_shader.glsl");
}

void EasyCore::setMaterialData(const std::vector<CoreMaterial>& materials) {
	m_MaterialData.reserve(materials.size());

	for (const CoreMaterial& coreMaterial : materials) {
		Material material{};
		material.color = coreMaterial.color;

		m_MaterialData.push_back(material);
	}
}

void EasyCore::setSphereData(const std::vector<CoreSphere>& spheres) {
	m_SphereData.reserve(spheres.size());

	for (const CoreSphere& coreSphere : spheres) {
		Sphere sphere{};
		sphere.center = coreSphere.center;
		sphere.radius = coreSphere.radius;
		sphere.materialIndex = coreSphere.materialIndex;

		m_SphereData.push_back(sphere);
	}
}

void EasyCore::setTriangleData(const std::vector<CoreTriangle>& triangles) {
	m_TriangleData.reserve(triangles.size());

	for (const CoreTriangle& coreTriangle : triangles) {
		Triangle triangle{};
		triangle.vertex0 = coreTriangle.vertex0;
		triangle.vertex1 = coreTriangle.vertex1;
		triangle.vertex2 = coreTriangle.vertex2;
		triangle.normal = coreTriangle.normal;
		triangle.materialIndex = coreTriangle.materialIndex;

		m_TriangleData.push_back(triangle);
	}
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

const HitRecord EasyCore::trace(const Ray& ray, uint32_t depth) const {
	float t; // Distance of current intersection test, to be compared to nearestT
	float nearestT = FLT_MAX; // Distance to nearest intersection
	glm::vec3 nearestIntersection;
	glm::vec3 nearestNormal;
	Material nearestMaterial;

	// Intersect spheres
	for (const Sphere& sphere : m_SphereData) {
		if (tmath::intersectSphere(ray, sphere.center, sphere.radius, t)) {
			// If the intersection is closer than what was previously found
			if (t < nearestT) {
				nearestT = t;
				nearestIntersection = ray.at(t);
				nearestNormal = glm::normalize(ray.at(t) - sphere.center);
				nearestMaterial = m_MaterialData[sphere.materialIndex];
			}
		}
	}

	// Intersect triangles
	for (const Triangle& triangle : m_TriangleData) {
		if (tmath::intersectTriangle(ray, triangle.vertex0, triangle.vertex1, triangle.vertex2, t)) {
			if (t < nearestT) {
				nearestT = t;
				nearestIntersection = ray.at(t);
				nearestNormal = triangle.normal;
				nearestMaterial = m_MaterialData[triangle.materialIndex];
			}
		}
	}

	// If we intersected any geometry
	if (nearestT < FLT_MAX) {
		nearestIntersection += nearestNormal * EPS; // Slightly offset the intersection to make sure it's outside of the geometry
		nearestT = glm::length(nearestIntersection - ray.origin()); // Recalculate the intersection distance after offsetting

		if (nearestMaterial.isEmissive()) {
			return { nearestMaterial.color, nearestT }; // If we hit a light, do not bounce any further
		} else if (depth > 1) {
			// Diffuse bounce
			glm::vec3 diffuseDirection = tmath::sphericalRand();

			if (glm::dot(diffuseDirection, nearestNormal) < 0.0f) {
				diffuseDirection = -diffuseDirection;
			}

			// const glm::vec3 diffuseDirection = nearestNormal + tmath::sphericalRand();
			const Ray diffuseRay(nearestIntersection, diffuseDirection);
			const HitRecord diffuseHitRecord = trace(diffuseRay, depth - 1);

			const glm::vec3 BRDF = nearestMaterial.color * INVPI;
			const float invPDF = 2.0f * FPI;

			const glm::vec3 L = diffuseHitRecord.color / (diffuseHitRecord.distance * diffuseHitRecord.distance); // Account for distance to the light
			const glm::vec3 E = L * glm::dot(diffuseDirection, nearestNormal) * invPDF;

			return { 2.0f * FPI * BRDF * E, nearestT };
		}
	}

	// If the ray did not hit any geometry it hits the skybox
	if (m_SkydomeData.size() > 0) {
		const float u = 1 + glm::atan(ray.direction().x, -ray.direction().z) * INVPI;
		const float v = acos(ray.direction().y) * INVPI;

		const size_t width = (size_t)round(u * m_SkydomeWidth);
		const size_t height = (size_t)round(v * m_SkydomeHeight);

		return { glm::clamp(m_SkydomeData[glm::min(height * m_SkydomeHeight * 2 + width, m_SkydomeData.size() - 1)], 0.0f, 1.0f), m_SkydomeRadius };
	}
	
	return { glm::vec3(0.04f, 0.03f, 0.08f), m_SkydomeRadius }; // If there is no skybox, return a pretty dark color
}

const HitRecord EasyCore::traceIS(const Ray& ray, uint32_t depth) const {
	float t; // Distance of current intersection test, to be compared to nearestT
	float nearestT = FLT_MAX; // Distance to nearest intersection
	glm::vec3 nearestIntersection;
	glm::vec3 nearestNormal;
	Material nearestMaterial;

	// Intersect spheres
	for (const Sphere& sphere : m_SphereData) {
		if (tmath::intersectSphere(ray, sphere.center, sphere.radius, t)) {
			// If the intersection is closer than what was previously found
			if (t < nearestT) {
				nearestT = t;
				nearestIntersection = ray.at(t);
				nearestNormal = glm::normalize(ray.at(t) - sphere.center);
				nearestMaterial = m_MaterialData[sphere.materialIndex];
			}
		}
	}

	// Intersect triangles
	for (const Triangle& triangle : m_TriangleData) {
		if (tmath::intersectTriangle(ray, triangle.vertex0, triangle.vertex1, triangle.vertex2, t)) {
			if (t < nearestT) {
				nearestT = t;
				nearestIntersection = ray.at(t);
				nearestNormal = triangle.normal;
				nearestMaterial = m_MaterialData[triangle.materialIndex];
			}
		}
	}

	// If we intersected any geometry
	if (nearestT < FLT_MAX) {
		nearestIntersection += nearestNormal * EPS; // Slightly offset the intersection to make sure it's outside of the geometry
		nearestT = glm::length(nearestIntersection - ray.origin()); // Recalculate the intersection distance after offsetting

		if (nearestMaterial.isEmissive()) {
			return { nearestMaterial.color, nearestT }; // If we hit a light, do not bounce any further
		} else if (depth > 1) {
			// Diffuse bounce
			const glm::vec3 diffuseDirection = nearestNormal + tmath::sphericalRand();
			const Ray diffuseRay(nearestIntersection, diffuseDirection);
			const HitRecord diffuseHitRecord = trace(diffuseRay, depth - 1);

			const glm::vec3 L = diffuseHitRecord.color / (diffuseHitRecord.distance * diffuseHitRecord.distance); // Account for distance to the light

			return { 2.0f * FPI * nearestMaterial.color * L, nearestT };
		}
	}

	// If the ray did not hit any geometry it hits the skybox
	if (m_SkydomeData.size() > 0) {
		const float u = 1 + glm::atan(ray.direction().x, -ray.direction().z) * INVPI;
		const float v = acos(ray.direction().y) * INVPI;

		const size_t width = (size_t)round(u * m_SkydomeWidth);
		const size_t height = (size_t)round(v * m_SkydomeHeight);

		return { glm::clamp(m_SkydomeData[glm::min(height * m_SkydomeHeight * 2 + width, m_SkydomeData.size() - 1)], 0.0f, 1.0f), m_SkydomeRadius };
	}

	return { glm::vec3(0.04f, 0.03f, 0.08f), m_SkydomeRadius }; // If there is no skybox, return a pretty dark color
}

const std::vector<uint8_t>& EasyCore::nextFrame() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	m_Shader.use();

	for (uint32_t y = 0; y < m_FrameHeight; y++) {
		for (uint32_t x = 0; x < m_FrameWidth; x++) {
			glm::vec3 color(0.0f, 0.0f, 0.0f);

			for (uint32_t c = 0; c < m_SamplesPerPixel; c++) {
				// Use a random offset for anti-aliasing
				float xOffset = dis(gen);
				float yOffset = dis(gen);

				const float u = (x + xOffset) / m_FrameWidth;
				const float v = (y + yOffset) / m_FrameHeight;

				Ray ray = m_Camera.getRay(u, v);
				color += traceIS(ray, 2).color;
			}
			
			color /= m_SamplesPerPixel;

			const uint32_t i = 3 * (y * m_FrameWidth + x);
			m_Frame[i + 0] = static_cast<uint8_t>(glm::clamp(256 * color.r, 0.0f, 255.99f));
			m_Frame[i + 1] = static_cast<uint8_t>(glm::clamp(256 * color.g, 0.0f, 255.99f));
			m_Frame[i + 2] = static_cast<uint8_t>(glm::clamp(256 * color.b, 0.0f, 255.99f));
		}
	}

	// Testing
	uint32_t totalR = 0;
	uint32_t totalG = 0;
	uint32_t totalB = 0;

	for (uint32_t i = 0; i < 3 * m_FrameHeight * m_FrameWidth; i += 3) {
		totalR += m_Frame[i + 0];
		totalG += m_Frame[i + 1];
		totalB += m_Frame[i + 2];
	}

	std::cout << "(" << m_SamplesPerPixel << "," << totalB << ")";

	return m_Frame;
}