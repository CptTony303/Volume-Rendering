#include <core/Scenes/volumeScene.h>
#include <glad/glad.h>
#include <core/framebuffer.h>
#include <core/texture3D.h>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <core/camera.h>
#include <core/GUI/volumeGUI.h>
#include <vector>

int g_width = 1500, g_height = 1200;

VolumeScene::VolumeScene(int width, int height)
{
	g_width = width;
	g_height = height;
}
void VolumeScene::initScene() {
	initTransformationMatrices();
	initVAOs();
	initFBOs();
	gui = VolumeGUI();

	trans_func_points_color.clear();
	trans_func_points_color.resize(gui.trans_func_points_color.size() * 2);
	for (int i = 0; i < gui.trans_func_points_color.size(); i++)
	{
		trans_func_points_color[i * 2] = gui.trans_func_points_color[i].x;
		trans_func_points_color[i * 2 + 1] = gui.trans_func_points_color[i].y;
	}
	initShaders();
}

void VolumeScene::renderScene()
{
	renderGUI();
	updateShaderValues();
	renderVolume();
}

void VolumeScene::processInput(GLFWwindow* window, float delta)
{
	if (camera.processInput(window, delta))
		accumulatedFrames = 0;
}

void VolumeScene::scroll_callback(double xoffset, double yoffset)
{
	camera.scroll_callback(xoffset, yoffset);
}
void VolumeScene::framebuffer_size_callback(int width, int height)
{
	g_width = width;
	g_height = height;
}


void VolumeScene::initTransformationMatrices()
{
	model = glm::mat4(1.0f);
	//model = glm::scale(model, glm::vec3(103, 94, 161));
	glm::vec3 cam_pos = glm::vec3(-3.0f, 0.0f, 0.0f);
	glm::vec3 cam_view_dir = glm::vec3(1.f, 0.f, 0.f);
	glm::vec3 cam_up_dir = glm::vec3(0.f, 1.f, 0.f);

	camera = Camera(cam_pos, cam_view_dir, cam_up_dir,
		45.f, float(g_width) / float(g_height), 0.1f, 100.f);
}

void VolumeScene::initVolumeVAO()
{
	// setting vertices and indices for a cube to render the volume in
	// the vertices also bind a 3D texture coordinate
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,
	};
	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3,

		1, 2, 5,
		2, 5, 6,

		0, 3, 4,
		3, 4, 7,

		0, 1, 4,
		1, 4, 5,

		2, 3, 7,
		2, 6, 7,

		4, 5, 6,
		4, 6, 7
	};
	// create and VAO for the Volume; private member variable to reuse it later
	glGenVertexArrays(1, &VAO_VOLUME);
	glBindVertexArray(VAO_VOLUME);

	// binding the vertices to the vao
	unsigned int vbo_volume;
	glGenBuffers(1, &vbo_volume);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_volume);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//binding the indices to the vao
	unsigned int ebo_volume;
	glGenBuffers(1, &ebo_volume);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_volume);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void VolumeScene::initFrameVAO()
{

	float frameVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	// Framebuffer VAO
	glGenVertexArrays(1, &VAO_FRAME);
	glBindVertexArray(VAO_FRAME);

	unsigned int vbo_frame;
	glGenBuffers(1, &vbo_frame);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_frame);
	glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), &frameVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void VolumeScene::initVAOs()
{
	initVolumeVAO();
	initFrameVAO();
}

void VolumeScene::initFBOs()
{
	FBO_VOLUME = Framebuffer(g_width, g_height);
	FBO_ACCUMULATION = Framebuffer(g_width, g_height);
	FBO_LAST_FRAME = Framebuffer(g_width, g_height);
}

void VolumeScene::initShaders()
{
	initVolumeShaders();
	initAccumulationShader();
	initCopyShader();
	initDebugShaders();
	updateShaderValues();
}

void VolumeScene::initVolumeShaders()
{
	//dynamic volume path and size
	Texture3D volumeData("C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/Assets/foot_256x256x256_uint8.raw", GL_TEXTURE3, glm::vec3(256, 256, 256));

	const char* vertPathMonteCarlo = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/monte_carlo.vert";
	const char* fragPathMonteCarlo = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/monte_carlo.frag";

	monteCarloShader = Shader(vertPathMonteCarlo, fragPathMonteCarlo);
	monteCarloShader.use();

	monteCarloShader.setInt("volume", 3);
	monteCarloShader.setInt("convergedFrame", 1);

	const char* vertPathRayMarch = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/volume.vert";
	const char* fragPathRayMarch = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/ray_marching.frag";

	rayMarchShader = Shader(vertPathRayMarch, fragPathRayMarch);
	rayMarchShader.use();

	rayMarchShader.setInt("volume", 3);
}

void VolumeScene::initAccumulationShader()
{
	const char* vertPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/copy.vert";
	const char* fragPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/accumulation.frag";

	accumulationShader = Shader(vertPath, fragPath);
	accumulationShader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBO_VOLUME.getTexture());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, FBO_LAST_FRAME.getTexture());

	accumulationShader.setInt("screenTexture", 0);
	accumulationShader.setInt("screenCopy", 2);
}

void VolumeScene::initCopyShader()
{
	const char* vertPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/copy.vert";
	const char* fragPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/copy.frag";

	copyShader = Shader(vertPath, fragPath);
	copyShader.use();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, FBO_ACCUMULATION.getTexture());

	copyShader.setInt("screenTexture", 1);
}

void VolumeScene::initDebugShaders()
{
	//transfer function shader
	const char* vertPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/copy.vert";
	const char* fragPath = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/transfer_function.frag";

	transferFunctionShader = Shader(vertPath, fragPath);
	transferFunctionShader.use();

	//volume debug shader
	const char* vertPathDebug = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/volume.vert";
	const char* fragPathDebug = "C:/Users/Anton/Privat/Projects/Programming/Cpp/Volume-Rendering/shaders/volume_debug.frag";

	volumeDebugShader = Shader(vertPathDebug, fragPathDebug);
	volumeDebugShader.use();
}

void VolumeScene::renderGUI()
{
	gui.renderGUI();
	if (gui.reset_accumulation_frames) {
		accumulatedFrames = 0;
		//std::cout << "\nColors:\n";
		//for each (glm::vec2 p in gui.trans_func_points_color)
		//{
		//	std::cout << "(" << p.x << "," << p.y << ") , ";
		//}
		//std::cout << std::endl;
		//std::cout << "Density:\n";
		//for each (glm::vec2 p in gui.trans_func_points_density)
		//{
		//	std::cout << "(" << p.x << "," << p.y << ") , ";
		//}
		//std::cout << "\n" << std::endl;
	}
}

void VolumeScene::renderVolume()
{
	glBindVertexArray(VAO_VOLUME);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //draw on screen

	switch (gui.methode) {
	case 0://ray marching
		accumulatedFrames = -1;
		rayMarchShader.use();
		break;
	case 1://monte carlo
		glBindFramebuffer(GL_FRAMEBUFFER, FBO_VOLUME.getID()); //draw into framebuffer
		monteCarloShader.use();
		break;
	case 3://transfer function
		accumulatedFrames = -1;
		glBindVertexArray(VAO_FRAME);
		transferFunctionShader.use();
		break;
	case 2:
	case 4://rng
		accumulatedFrames = -1;
		volumeDebugShader.use();
		break;
	}
	glClearColor(0.f, 0.f, 0.f, 1.0f); //set clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	if (gui.methode != 3) {

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	if (gui.methode == 1) { //if monte carlo
		VolumeScene::accumulateFrames(); //accumulate frames
	}
}

void VolumeScene::accumulateFrames()
{
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(VAO_FRAME);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_ACCUMULATION.getID());
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	accumulationShader.use();

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO_LAST_FRAME.getID());
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	copyShader.use();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	accumulatedFrames++;
}
void VolumeScene::updateShaderValues()
{
	std::vector <float> last_trans_func_points_color;
	if (gui.reset_accumulation_frames) {

		for (int i = 0; i < trans_func_points_color.size(); i++)
		{
			last_trans_func_points_color.push_back(trans_func_points_color[i]);
		}
		if (trans_func_points_color.size() != gui.trans_func_points_color.size() * 2) {
			trans_func_points_color.clear();
			trans_func_points_color.shrink_to_fit();
			trans_func_points_color.resize(gui.trans_func_points_color.size() * 2);
		}
		for (int i = 0; i < gui.trans_func_points_color.size(); i++)
		{
			trans_func_points_color[i * 2] = gui.trans_func_points_color[i].x;
			trans_func_points_color[i * 2 + 1] = gui.trans_func_points_color[i].y;
		}
	}
	std::vector <float> trans_func_points_density;
	for each (glm::vec2 p in gui.trans_func_points_density)
	{
		trans_func_points_density.push_back(p.x);
		trans_func_points_density.push_back(p.y);
	}

	rayMarchShader.use();
	rayMarchShader.setMat4("model", model);
	rayMarchShader.setMat4("view", camera.getViewMatrix());
	rayMarchShader.setMat4("projection", camera.getProjectionMatrix());
	rayMarchShader.setFloat("stepSize", gui.stepSize);
	rayMarchShader.setInt("numberOfColorPoints", gui.trans_func_points_color.size());
	rayMarchShader.setListVec2("transfer_function_color", trans_func_points_color);
	rayMarchShader.setInt("numberOfDensityPoints", gui.trans_func_points_density.size());
	rayMarchShader.setListVec2("transfer_function_density", trans_func_points_density);

	monteCarloShader.use();
	monteCarloShader.setMat4("model", model);
	monteCarloShader.setMat4("view", camera.getViewMatrix());
	monteCarloShader.setMat4("projection", camera.getProjectionMatrix());

	monteCarloShader.setFloat("screenSize_x", (float)g_width);
	monteCarloShader.setFloat("screenSize_y", (float)g_height);

	monteCarloShader.setInt("samplesPerFrame", gui.samplesPerFrame);
	monteCarloShader.setFloat("randomizer", (float)glfwGetTime());
	monteCarloShader.setFloat("brightness", gui.brightness);
	monteCarloShader.setInt("numberOfColorPoints", gui.trans_func_points_color.size());
	monteCarloShader.setListVec2("transfer_function_color", trans_func_points_color);
	monteCarloShader.setInt("numberOfDensityPoints", gui.trans_func_points_density.size());
	monteCarloShader.setListVec2("transfer_function_density", trans_func_points_density);

	monteCarloShader.setInt("lastNumberOfColorPoints", last_trans_func_points_color.size() / 2);
	monteCarloShader.setListVec2("transfer_function_color", last_trans_func_points_color);
	monteCarloShader.setBool("trans_func_changed", gui.reset_accumulation_frames);


	accumulationShader.use();
	accumulationShader.setInt("runs", accumulatedFrames);
	accumulationShader.setInt("samplesPerRun", gui.samplesPerFrame);

	transferFunctionShader.use();
	transferFunctionShader.setInt("numberOfColorPoints", gui.trans_func_points_color.size());
	transferFunctionShader.setListVec2("transfer_function_color", trans_func_points_color);
	transferFunctionShader.setInt("numberOfDensityPoints", gui.trans_func_points_density.size());
	transferFunctionShader.setListVec2("transfer_function_density", trans_func_points_density);

	volumeDebugShader.use();
	volumeDebugShader.setMat4("model", model);
	volumeDebugShader.setMat4("view", camera.getViewMatrix());
	volumeDebugShader.setMat4("projection", camera.getProjectionMatrix());
	volumeDebugShader.setFloat("randomizer", (float)glfwGetTime());
	volumeDebugShader.setInt("methode", gui.methode);

	gui.reset_accumulation_frames = false;
}
