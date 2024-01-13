#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Camera {
public:
	Camera();
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up_direction,
		float view_angle, float image_ratio, float min_distance, float max_distance);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	bool processInput(GLFWwindow* window, float delta);

private:
	void update_view();
	void reset_view();
	float _speed = 5.f, _sensitivity = 1.f;

	glm::mat4 view, projection;
	glm::vec3 _position, _view_direction, _up_direction, _init_position, _init_view_direction;
	float _view_angle, _init_view_angle;
	double _last_mouse_position_x, _last_mouse_position_y;
};

#endif