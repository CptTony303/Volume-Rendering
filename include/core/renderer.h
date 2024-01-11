#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <core/shader.h>
#include <glm/glm.hpp>
#include <core/Scenes/volumeScene.h>

class Renderer
{
public:
	Renderer(GLFWwindow* window);
	//void loadScene(Scene* scene);
	void renderScene();
	void unusedRender();
	void init();
	void unusedInit();
private:
	VolumeScene scene;
	GLFWwindow* _window;
	unsigned int VAO, VAO_framebuffer, fbo, fbo_switch, fbo_copy, VAO_copy;
	Shader* myShader, * framebufferShader, * copyShader;
	glm::vec3 _cam_pos;
	glm::vec3 _cam_rot;
	bool oszillate = false;
	bool spin = false;
	float isovalue = 0.f;
	int brightness = 1;
	float stepSize = 0.01;
	int methode = 0;
	int runs = 0;
};