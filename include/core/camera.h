#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Camera {
public:
	Camera(float imageRatio);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

private:
	void update_view();
	void update_projection();
	float  _view_angle, _image_ratio, _min_distance, _max_distance;

	glm::mat4 view, projection;
	glm::vec3 _position, _view_direction, _up_direction;
};

#endif