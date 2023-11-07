#include <core/object.h>
#include <glm/glm.hpp>
#include <core/ray.h>


class Sphere {
public:
	Sphere(glm::vec3 center, float radius): _center(center), _radius(radius) {};
	bool intersectWithRay(Ray ray, float* t);
private:
	glm::vec3 _center;
	float _radius;
};
