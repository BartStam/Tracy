#pragma once

class Camera {
public:
	Camera()
		: m_Origin(glm::vec3(0.0f))
		, m_LowerLeftCorner(glm::vec3(-0.5, -0.5, 1.0f))
		, m_Horizontal(glm::vec3(1.0f, 0.0f, 0.0f))
		, m_Vertical(glm::vec3(0.0f, 1.0f, 0.0f)) {}

	Camera(uint32_t frameWidth, uint32_t frameHeight, float fieldOfView, const glm::vec3& origin, const glm::vec3& direction) : m_Origin(origin) {
		glm::vec3 normalizedDirection = glm::normalize(direction); // Make sure the direction vector is normalized before use
		glm::vec3 initialDirection = glm::vec3(0.0f, 0.0f, 1.0f); // Direction that the viewport faces prior to rotation

		float viewportHeight = frameHeight;
		float viewportWidth = frameWidth;

		// First establish the viewport centered in the origin, facing in direction (0, 0, 1)
		// These are defined as vec4 so they can be transformed using a 4x4 rotation matrix
		glm::vec4 lowerLeftCorner = glm::vec4(-viewportWidth * 0.5f, -viewportHeight * 0.5f, 0.0f, 1.0f);
		glm::vec4 horizontal = glm::vec4(viewportWidth, 0.0f, 0.0f, 0.0f);
		glm::vec4 vertical = glm::vec4(0.0f, viewportHeight, 0.0f, 0.0f);

		// Rotate the viewport to face the desired direction
		// Only do this if it is not already facing said direction
		if (!glm::all(glm::equal(initialDirection, normalizedDirection))) {
			glm::vec3 rotationVector = glm::cross(initialDirection, normalizedDirection);
			float rotationAngle = glm::radians(90.0f - glm::dot(initialDirection, normalizedDirection) * 90.0f);
			glm::mat4 rotationMatrix = glm::rotate(rotationAngle, rotationVector);

			lowerLeftCorner = rotationMatrix * lowerLeftCorner;
			horizontal = rotationMatrix * horizontal;
			vertical = rotationMatrix * vertical;
		}
		
		m_LowerLeftCorner = glm::vec3(lowerLeftCorner);
		m_Horizontal = glm::vec3(horizontal);
		m_Vertical = glm::vec3(vertical);

		// Calculate focal length based on field of view
		float focalLength = (viewportWidth * 0.5f) / glm::tan(M_PI * fieldOfView / 360);

		// Translate viewport to its desired origin + focal length
		m_LowerLeftCorner += origin;
		m_LowerLeftCorner += focalLength * normalizedDirection;
	}

	Ray getRay(float u, float v) {
		return Ray(m_Origin, m_LowerLeftCorner + u * m_Horizontal + v * m_Vertical - m_Origin);
	}

	void setFieldOfView(float degrees) {

	}

	void tilt(float degrees) {

	}

	void pan(float degrees) {

	}

	void zoom(float distance) {

	}

	const glm::vec3& origin() { return m_Origin; }
	const glm::vec3& lowerLeftCorner() { return m_LowerLeftCorner; }
	const glm::vec3& horizontal() { return m_Horizontal; }
	const glm::vec3& vertical() { return m_Vertical; }

private:
	// World space positions
	glm::vec3 m_Origin;
	glm::vec3 m_LowerLeftCorner;
	glm::vec3 m_Horizontal;
	glm::vec3 m_Vertical;
};
