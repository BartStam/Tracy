#pragma once

class Camera {
public:
	Camera() = default;

	Camera(uint32_t frameWidth, uint32_t frameHeight, float fieldOfView, const glm::vec3& origin, const glm::vec3& direction)
		: m_ViewportWidth((float)frameWidth)
		, m_ViewportHeight((float)frameHeight)
		, m_FieldOfView(fieldOfView)
		, m_Position(origin)
		, m_Direction(glm::normalize(direction)) {
		setPoints();
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
		glm::vec3 rotationAxis = glm::normalize(glm::cross(m_Direction, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec newDirection = glm::rotate(m_Direction, angle, rotationAxis);
		float d = glm::dot(newDirection, glm::normalize(glm::vec3(newDirection.x, 0.0f, newDirection.z)));

		// Restrict vertical camera angle so it doesn't rotate upside down
		if (d > 0.2f) {
			m_Direction = newDirection;
			setPoints();
		}
	}

	// Pan the camera by an angle (in radians)
	void pan(float angle) {
		m_Direction = glm::rotate(m_Direction, angle, glm::vec3(0.0f, -1.0f, 0.0f));
		setPoints();
	}

	// Move the camera by an offset in object space
	// x component is left (negative)/right (positive)
	// y component is backward (negative)/forward (positive)
	void move(glm::vec2 offset) {
		m_Position += offset.x * glm::normalize(glm::cross(m_Direction, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_Position += offset.y * m_Direction;
		setPoints();
	}

	const glm::vec3& position() { return m_Position; }
	const glm::vec3& lowerLeftCorner() { return m_LowerLeftCorner; }
	const glm::vec3& horizontal() { return m_Horizontal; }
	const glm::vec3& vertical() { return m_Vertical; }
private:
	// Sets m_LowerLeftCorner, m_Horizontal, and m_Vertical based on m_ViewportWidth, m_ViewportHeight, m_FieldOfView,
	// m_Position, and m_Direction. This function is called in tilt(), pan(), move(), and setFieldOfView().
	void setPoints() {
		const glm::vec3 perpendicularH = glm::normalize(glm::cross(m_Direction, glm::vec3(0.0f, 1.0f, 0.0f)));
		const glm::vec3 perpendicularV = glm::normalize(glm::cross(m_Direction, -perpendicularH));

		m_Horizontal = m_ViewportWidth * perpendicularH;
		m_Vertical = m_ViewportHeight * perpendicularV;

		// Calculate focal length based on field of view
		const float focalLength = (m_ViewportWidth * 0.5f) / glm::tan(FPI * m_FieldOfView / 360);

		m_LowerLeftCorner = focalLength * m_Direction - 0.5f * m_Horizontal - 0.5f * m_Vertical;
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
