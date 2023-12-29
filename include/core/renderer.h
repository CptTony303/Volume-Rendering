#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <core/shader.h>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer(GLFWwindow* window);
	//void loadScene(Scene* scene);
	void renderScene();
	void init();
private:
	GLFWwindow* _window;
	unsigned int VAO;
	Shader* myShader;
	glm::vec3 _cam_pos;
	glm::vec3 _cam_rot;
	bool oszillate = false;
	bool spin = false;
	float isovalue = 0.45f;
	int brightness = 15;
	float stepSize = 0.01;
	int methode = 0;
};