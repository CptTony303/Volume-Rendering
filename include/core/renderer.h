#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <core/scene.h>
#include <core/shader.h>

class Renderer
{
public:
	Renderer(GLFWwindow* window);
	void loadScene(Scene* scene);
	void renderScene();
	void init();
private:
	GLFWwindow* _window;
	unsigned int VAO;
	Shader* myShader;
};