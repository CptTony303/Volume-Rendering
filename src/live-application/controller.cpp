#include <live-application/controller.h>
#include <glm/gtx/string_cast.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

}

void Controller::init(GLFWwindow* glfw_window, Renderer* renderer)
{
	//glfwSetKeyCallback(glfw_window, func);
	//glfwSetMouseButtonCallback(glfw_window,func);
	glfwSetScrollCallback(glfw_window, scroll_callback);
	glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);
	this->glfw_window = glfw_window;
	this->renderer = renderer;
	double mouse_x, mouse_y;
	glfwGetCursorPos(glfw_window, &mouse_x, &mouse_y);
	lastMouse = glm::vec2(mouse_x, mouse_y);
}

bool Controller::processInput(float delta)
{
	bool modified = false;
	glm::vec3 translation = glm::vec3(0.f);

	//exit
	if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(glfw_window, true);

	//translation
	if (glfwGetKey(glfw_window, GLFW_KEY_W) == GLFW_PRESS) {
		translation.y -= 1;
	}
	if (glfwGetKey(glfw_window, GLFW_KEY_S) == GLFW_PRESS) {
		translation.y += 1;
	}
	if (glfwGetKey(glfw_window, GLFW_KEY_A) == GLFW_PRESS) {
		translation.z += 1;
	}
	if (glfwGetKey(glfw_window, GLFW_KEY_D) == GLFW_PRESS) {
		translation.z -= 1;
	}
	if (glm::length(translation) > 0.1) {
		modified = true;
		translation = glm::normalize(translation);
		translation *= delta * cameraSpeed;
		renderer->setCameraPosition(glm::translate(renderer->getCameraPosition(), translation));
	}
	//rotation
	double mouse_x, mouse_y;
	glfwGetCursorPos(glfw_window, &mouse_x, &mouse_y);
	if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		float angle_x = (mouse_x - lastMouse.x) * delta * sensitivity;
		float angle_y = (mouse_y - lastMouse.y) * delta * sensitivity;
		glm::mat4 matrix = renderer->getVolumePosition();
		glm::vec3 xAxisInModel = glm::inverse(matrix)*glm::vec4(0, 1, 0, 0);
		matrix = glm::rotate(matrix, glm::radians(angle_x), xAxisInModel);
		glm::vec3 yAxisInModel = glm::inverse(matrix) * glm::vec4(0, 0, 1, 0);
		matrix = glm::rotate(matrix, glm::radians(angle_y), yAxisInModel);
		renderer->setVolumePosition(matrix);
		modified = true;
	}
	lastMouse = glm::vec2(mouse_x, mouse_y);

	//if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
	//	reset_camera();
	//	modified = true;
	//}

	//if (modified) {
	//	update_view();
	//}

	////change zoom
	//if (g_view_angle != _view_angle) {
	//	//view_angle_mutex.lock();
	//	_view_angle = g_view_angle;
	//	//view_angle_mutex.unlock();
	//	update_projection();
	//	modified = true;
	//}

	return modified;
}
