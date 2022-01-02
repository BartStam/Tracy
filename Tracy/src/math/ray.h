#pragma once

class Ray {
public:
	Ray() = default;
	Ray(const glm::vec3& origin, const glm::vec3& direction)
		: m_Origin(origin)
		, m_Direction(glm::normalize(direction)) {
	}

	const glm::vec3& origin() const { return m_Origin; }
	const glm::vec3& direction() const { return m_Direction; }

	glm::vec3 at(float t) const {
		return origin() + t * direction();
	}

private:
	glm::vec3 m_Origin = glm::vec3(0.0f);
	glm::vec3 m_Direction = glm::vec3(0.0f);
};
