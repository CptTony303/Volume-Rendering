#include <core/object.h>
#include <glm/vec3.hpp>


class Sphere {
public:
	Sphere(glm::vec3 center, float radius): _center(center), _radius(radius) {};
private:
	glm::vec3 _center;
	float _radius;
};
