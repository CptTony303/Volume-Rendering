#ifndef RAY_H
#define RAY_H

#include <glm/vec3.hpp>

class Ray {
public:
	Ray(glm::vec3 origin, glm::vec3 direction) : _origin(origin), _direction(direction) {};
	glm::vec3 direction() { return _direction; };
	glm::vec3 origin() { return _origin; };
private:
	glm::vec3 _direction;
	glm::vec3 _origin;
};

#endif