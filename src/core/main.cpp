#include <core/main.h>
#include <live-application/live-application.h>
#include <tester/tester.h>

// am besten mit argument f�r entweder live app oder tester
int main() {
	Tester();
	//LiveApplication().start();
	//Window* window = Window::getInstance(); // window nur f�r live app
	//window->initWindow();
	//window->startRenderLoop();
	return 0;
}