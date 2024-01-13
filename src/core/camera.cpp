#include <core/camera.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#define GLFW_INCLUDE_NONE
#include <core/window.h>

//callbacks for glfw window
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	std::cout << "Holy shit, it works!!" << std::endl;
}

Camera::Camera()
{
}

Camera::Camera(glm::vec3 position, glm::vec3 view_direction, glm::vec3 up_direction,
	float view_angle, float image_ratio, float min_distance, float max_distance)
{
	_position = position;
	_view_direction = glm::normalize(view_direction);
	_up_direction = glm::normalize(up_direction);
	_view_angle = image_ratio;
	_init_view_direction = _view_direction;
	_init_position = _position;
	_init_view_angle = _view_angle;

	update_view();
	//Window::getInstance()->registerCallback(Window::SCROLL, scroll_callback);
	projection = glm::perspective(glm::radians(_view_angle), image_ratio, min_distance, max_distance);
}

glm::mat4 Camera::getViewMatrix()
{
	return view;
}

glm::mat4 Camera::getProjectionMatrix()
{
	return projection;
}

bool Camera::processInput(GLFWwindow* window, float delta)
{
	bool modified = false;

	//position movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		_position += delta * _speed * _view_direction * 4.f;
		modified = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		_position -= delta * _speed * _view_direction * 4.f;
		modified = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		_position -= glm::normalize(glm::cross(_view_direction, _up_direction)) * delta * _speed;
		modified = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		_position += glm::normalize(glm::cross(_view_direction, _up_direction)) * delta * _speed;
		modified = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glm::vec3 right_dir = glm::normalize(glm::cross(_view_direction, _up_direction));
		glm::vec3 up_dir = glm::normalize(glm::cross(right_dir, _view_direction));
		_position += up_dir * delta * _speed;
		modified = true;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		glm::vec3 right_dir = glm::normalize(glm::cross(_view_direction, _up_direction));
		glm::vec3 up_dir = glm::normalize(glm::cross(right_dir, _view_direction));
		_position -= up_dir * delta * _speed;
		modified = true;
	}
	//view direction change
	double current_mouse_position_x, current_mouse_position_y;
	glfwGetCursorPos(window, &current_mouse_position_x, &current_mouse_position_y);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		float yaw, pitch;
		yaw = atan2(_view_direction.z, _view_direction.x);
		pitch = -asin(-_view_direction.y);

		yaw = glm::degrees(yaw);
		pitch = glm::degrees(pitch);

		float x_offset = current_mouse_position_x - _last_mouse_position_x;
		float y_offset = _last_mouse_position_y - current_mouse_position_y;

		yaw += x_offset * _sensitivity * delta;
		pitch += y_offset * _sensitivity * delta;

		//if (pitch > 89.0f)
		//	pitch = 89.0f;
		//if (pitch < -89.0f)
		//	pitch = -89.0f;

		_view_direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		_view_direction.y = sin(glm::radians(pitch));
		_view_direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		modified = true;
	}
	_last_mouse_position_x = current_mouse_position_x;
	_last_mouse_position_y = current_mouse_position_y;

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		reset_view();
		modified = true;
	}

	if (modified) {
		update_view();
	}
	return modified;
}

void Camera::update_view()
{
	_view_direction = glm::normalize(_view_direction);
	view = glm::lookAt(_position, _position + _view_direction, _up_direction);
}

void Camera::reset_view()
{
	_view_direction = _init_view_direction;
	_position = _init_position;
}
