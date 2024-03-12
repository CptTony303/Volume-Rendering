#include <core/camera.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#define GLFW_INCLUDE_NONE
#include <core/window.h>
#include <atomic>
#include <mutex>

Camera::Camera(float imageRatio)
{
	glm::vec3 cam_pos = glm::vec3(-3.0f, 0.0f, 0.0f);
	glm::vec3 cam_view_dir = glm::vec3(1.f, 0.f, 0.f);
	glm::vec3 cam_up_dir = glm::vec3(0.f, 1.f, 0.f);

	_position = cam_pos;
	_view_direction = glm::normalize(cam_view_dir);
	_up_direction = glm::normalize(cam_up_dir);
	_view_angle = 45.f;
	_image_ratio = imageRatio;
	_min_distance = 0.1f;
	_max_distance = 100.f;

	update_view();
	update_projection();
}

glm::mat4 Camera::getViewMatrix()
{
	return view;
}

glm::mat4 Camera::getProjectionMatrix()
{
	return projection;
}
void Camera::update_view()
{
	_view_direction = glm::normalize(_view_direction);
	view = glm::lookAt(_position, _position + _view_direction, _up_direction);
}

void Camera::update_projection()
{
	projection = glm::perspective(glm::radians(_view_angle), _image_ratio, _min_distance, _max_distance);
}


//move all input to live-app
/*
bool Camera::processInput(GLFWwindow* window, float delta)
{
	bool modified = false;

	//position movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		_position += delta * _speed * _view_direction * _z_dir_speedup;
		modified = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		_position -= delta * _speed * _view_direction * _z_dir_speedup;
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
		reset_camera();
		modified = true;
	}

	if (modified) {
		update_view();
	}

	//change zoom
	if (g_view_angle != _view_angle) {
		//view_angle_mutex.lock();
		_view_angle = g_view_angle;
		//view_angle_mutex.unlock();
		update_projection();
		modified = true;
	}

	return modified;
}
void Camera::scroll_callback(double xoffset, double yoffset)
{
	//view_angle_mutex.lock();
	g_view_angle -= (float)yoffset;
	if (g_view_angle < 1.f)
		g_view_angle = 1.f;
	if (g_view_angle > 179.0f)
		g_view_angle = 179.0f;
	
	//view_angle_mutex.unlock();
}

void Camera::reset_camera()
{
	//view
	_view_direction = _init_view_direction;
	_position = _init_position;
	update_view();
	//projection
	_view_angle = _init_view_angle;
	g_view_angle = _view_angle;
	_min_distance = _init_min_distance;
	_max_distance = _init_max_distance;
	update_projection();
}

*/