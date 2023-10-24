#include <core/Objects/Sphere.h>
#include <core/ray.h>
#include <glm/glm.hpp>

bool Sphere::intersectWithRay (Ray ray,
    float* t)                       // output parameter which contains distance to the hit point
{
	auto vpc = _center - ray.origin();
	auto t0 = glm::dot(vpc, ray.direction());
	auto y = glm::length(_center - (ray.origin() + t0 * ray.direction()));
	auto x = sqrtf((_radius * _radius) - (y * y));
	auto t1 = t0 - x;
	auto t2 = t0 + x;

	if (t1 < 0 && t2 < 0) {
		return false;
	}
	if (t1 < 0) {
		*t = t2;
		return true;
	}
	*t = t1;
	return true;
}