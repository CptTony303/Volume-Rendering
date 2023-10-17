#include <GLFW/glfw3.h>
#include <core/scene.h>

class Renderer
{
public:
	Renderer(GLFWwindow* window);
	void loadScene(Scene* scene);
	void renderScene();

private:
	GLFWwindow* _window;
};