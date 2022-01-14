#pragma once

#define EPS std::numeric_limits<float>::epsilon()
#define FPI glm::pi<float>()
#define INVPI glm::one_over_pi<float>()

namespace tmath {
    inline bool intersectSphere(const Ray& ray, const glm::vec3& center, float radius, float& t) {
        const glm::vec3 oc = ray.origin() - center;

        const float a = glm::dot(ray.direction(), ray.direction());
        const float b = 2.0f * glm::dot(oc, ray.direction());
        const float c = glm::dot(oc, oc) - radius * radius;
        const float discriminant = b * b - 4 * a * c;

        // No intersection
        if (discriminant < 0) {
            return false;
        }

        // Single intersection
        if (discriminant == 0) {
            t = -0.5f * b / a;

            if (t < 0) { return false; } // The intersection is behind the camera

            return true;
        }

        // Two intersections
        const float q = (b > 0) ? -0.5f * (b + sqrtf(discriminant)) : -0.5f * (b - sqrtf(discriminant));

        t = q / a;
        float t1 = c / q;

        // Make t is the nearest intersection
        if (t > t1) {
            std::swap(t, t1);
        }

        if (t < 0) {
            if (t1 < 0) {
                return false; // The sphere is entirely behind the camera
            }

            t = t1; // We are inside the sphere, only t1 is in front of the camera
            return true;
        }

        return true; // The sphere is in front of the camera
	}

    // Möller–Trumbore intersection algorithm
    // Source: https://wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    inline bool intersectTriangle(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) {
        const glm::vec3 edge1 = v1 - v0;
        const glm::vec3 edge2 = v2 - v0;
        const glm::vec3 h = glm::cross(ray.direction(), edge2);
        const float a = glm::dot(edge1, h);

        if (glm::abs(a) < EPS) {
            return false; // Ray is parallel to the triangle
        }
            
        const float f = 1.0f / a;
        const glm::vec3 s = ray.origin() - v0;
        const float u = f * glm::dot(s, h);

        if (u < 0.0 || u > 1.0) {
            return false;
        }
            
        const glm::vec3 q = glm::cross(s, edge1);
        const float v = f * glm::dot(ray.direction(), q);

        if (v < 0.0 || u + v > 1.0) {
            return false;
        }
            
        // At this stage we can compute t to find out where the intersection point is on the line.
        t = f * glm::dot(edge2, q);

        if (t > EPS) {
            return true;
        } else {
            return false; // Line intersection but no ray intersection.
        } 
    }

    inline glm::vec3 triangleNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
        return glm::normalize(glm::cross(v1 - v0, v2 - v0));
    }

    inline glm::vec3 randomPointOnTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
        float a = glm::linearRand(0.0f, 1.0f);
        float b = glm::linearRand(0.0f, 1.0f);

        if (a + b > 1.0f) { a = 1 - a; b = 1 - b; }
        return v0 + a * (v1 - v0) + b * (v2 - v0);
    }
}