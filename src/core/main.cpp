#include <core/main.h>
#include <live-application/live-application.h>
#include <tester/tester.h>

// am besten mit argument für entweder live app oder tester
int main() {
	Tester();
	//LiveApplication().start();
	//Window* window = Window::getInstance(); // window nur für live app
	//window->initWindow();
	//window->startRenderLoop();
	return 0;
}