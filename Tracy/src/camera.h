#pragma once

class Camera {
public:
	Camera() = default;

	Camera(uint32_t frameWidth, uint32_t frameHeight, float fieldOfView, const glm::vec3& origin, const glm::vec3& direction)
		: m_ViewportWidth(static_cast<float>(frameWidth))
		, m_ViewportHeight(static_cast<float>(frameHeight))
		, m_FieldOfView(fieldOfView)
		, m_Position(origin)
		, m_Direction(glm::normalize(direction)) {
		const glm::vec3 perpendicularH = glm::normalize(glm::cross(m_Direction, glm::vec3(0.0f, 1.0f, 0.0f)));
		const glm::vec3 perpendicularV = glm::normalize(glm::cross(m_Direction, -perpendicularH));

		m_Horizontal = (float)frameWidth * perpendicularH;
		m_Vertical = (float)frameHeight * perpendicularV;

		// Calculate focal length based on field of view
		const float focalLength = (frameWidth * 0.5f) / glm::tan(FPI * fieldOfView / 360);

		m_LowerLeftCorner = focalLength * m_Direction - 0.5f * m_Horizontal - 0.5f * m_Vertical;
	}

	// Return the ray that passes from the camera position through a (u, v) coordinate of the viewport
	Ray getRay(float u, float v) {
		return Ray(m_Position, m_LowerLeftCorner + u * m_Horizontal + v * m_Vertical - m_Position);
	}

	void setFieldOfView(float degrees) {
		m_FieldOfView = fmod(degrees, 359.0f);
	}

	// Tilt the camera by an angle (in radians)
	// Positive values tilt the camera upwards
	void tilt(float angle) {

	}

	// Pan the camera by an angle (in radians)
	void pan(float angle) {

	}

	const glm::vec3& position() { return m_Position; }
	const glm::vec3& lowerLeftCorner() { return m_LowerLeftCorner; }
	const glm::vec3& horizontal() { return m_Horizontal; }
	const glm::vec3& vertical() { return m_Vertical; }
private:
	// Sets m_LowerLeftCorner, m_Horizontal, and m_Vertical based on m_ViewportWidth, m_ViewportHeight, m_FieldOfView,
	// m_Origin, and m_Direction. This function is called in tilt(), pan(), move(), and setFieldOfView().
	void setPoints(float origin, float tiltAngle) {

	}
	
	float m_ViewportWidth;
	float m_ViewportHeight;
	float m_FieldOfView;
	glm::vec3 m_Position;
	glm::vec3 m_Direction;

	glm::vec3 m_LowerLeftCorner;
	glm::vec3 m_Horizontal;
	glm::vec3 m_Vertical;
};
