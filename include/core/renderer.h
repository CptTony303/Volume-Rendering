#ifndef RENDERER_H
#define RENDERER_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <core/Scenes/volumeScene.h>
#include <core/shader.h>
#include <core/framebuffer.h>
#include <vector>
#include <core/texture3D.h>
#include <json/json.hpp>
class Renderer
{
public:
	enum TransferFunctionType {COLOR, TRANSPARENCY};
	enum RenderMethods {MONTE_CARLO, RAY_MARCHING};

	Renderer(int width, int height) : width(width),
		height(height),
		scene(VolumeScene(float(width) / float(height))),
		shaders(4),
		framebuffers(3),
		FrameVAO(0),
		method(MONTE_CARLO),
		accumulatedFrames(-1),
		stepSize(0.1),
		samplesPerFrame(10),
		transferFunctions(2),
		useControlVariate(false),
		isControlVariateSet(false){
	}
	void renderScene();
	void renderVolume();
	void init();

	void setVolumeData(Texture3D volumeData);
	void setVolumePosition(glm::mat4 modelMatrix);
	void setCameraPosition(glm::mat4 viewMatrix);
	void setCameraOptions();
	void setRenderMethod(RenderMethods method);
	void setTransferFunction(std::vector <float> transferFunction, TransferFunctionType type);

	void setStepSize(float stepSize);
	void setSampleSize();
	void setControlVariate();
	void deleteControlVariate();
	void setUseControlVariate(bool useIt);
	void resetAccumulatedFrames();

	glm::mat4 getCameraPosition();
	glm::mat4 getVolumePosition();

	void saveDataToFile(std::string folder);

	void saveImageToFile(std::string fileName, std::string folder);


private:
	void initFrameVAO();
	void initFBOs();
	void initVolumeFBO();
	void initTransferFunctions();
	void initShaders();
	void initVolumeShaders();
	void initAccumulationShader();
	void initCopyShader();
	void initDebugShaders();
	void updateShaderValues();

	void accumulateFrames();

	struct ControlVariate {
		unsigned int texture;
		std::vector <float> transferFunctionColor;
		std::vector <float> transferFunctionDensity;
		glm::mat4 volumePosition;
		int runs;
	};

	int width, height;
	VolumeScene scene;
	enum FramebufferType {VOLUME, ACCUMULATION, LAST_FRAME, CONTROL_VARIATE};
	enum ShaderType {MC, RM, COPY, ACC};
	std::vector <Shader> shaders;
	std::vector <Framebuffer> framebuffers;
	unsigned int FrameVAO;
	RenderMethods method;
	int accumulatedFrames;
	float stepSize;
	int samplesPerFrame;
	std::vector <std::vector <float> > transferFunctions;
	bool useControlVariate;
	bool isControlVariateSet;
	ControlVariate controlVariate;
};
using json = nlohmann::json;
class JSONSerializer {
public:
	// Funktion zum Speichern der Daten
	void saveData(const std::vector<float>& transferFunctionColor,
		const std::vector<float>& transferFunctionDensity,
		const std::vector<float>& transferFunctionColor_CV,
		const std::vector<float>& transferFunctionDensity_CV,
		const int &runs,
		const int& runs_CV,
		const glm::mat4& volumePosition,
		const std::string& filename) {
		json data;
		data["transferFunctionColor"] = transferFunctionColor;
		data["transferFunctionDensity"] = transferFunctionDensity;
		data["volumePosition"] = {
			{volumePosition[0][0], volumePosition[0][1], volumePosition[0][2], volumePosition[0][3]},
			{volumePosition[1][0], volumePosition[1][1], volumePosition[1][2], volumePosition[1][3]},
			{volumePosition[2][0], volumePosition[2][1], volumePosition[2][2], volumePosition[2][3]},
			{volumePosition[3][0], volumePosition[3][1], volumePosition[3][2], volumePosition[3][3]}
		};
		data["runs_CV"] = runs;
		data["transferFunctionColor_CV"] = transferFunctionColor_CV;
		data["transferFunctionDensity_CV"] = transferFunctionDensity_CV;
		data["runs_CV"] = runs_CV;
		std::ofstream file(filename);
		file << data.dump(4); // pretty-print mit 4 Leerzeichen Einrückung
	}
	// Funktion zum Laden der Daten
	void loadData(std::vector<float>& transferFunctionColor,
		std::vector<float>& transferFunctionDensity,
		std::vector<float>& transferFunctionColor_CV,
		std::vector<float>& transferFunctionDensity_CV,
		int& runs,
		int& runs_CV,
		glm::mat4& volumePosition,
		const std::string& filename) {
		std::ifstream file(filename);
		if (!file) {
			std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
			return;
		}
		json data;
		file >> data;
		transferFunctionColor = data["transferFunctionColor"].get<std::vector<float>>();
		transferFunctionDensity = data["transferFunctionDensity"].get<std::vector<float>>();
		transferFunctionColor_CV = data["transferFunctionColor_CV"].get<std::vector<float>>();
		transferFunctionDensity_CV = data["transferFunctionDensity_CV"].get<std::vector<float>>();
		runs_CV = data["runs_CV"].get<int>();
		runs = data["runs_CV"].get<int>();
		// Rückrechnung von std::vector<std::vector<float>> zu glm::mat4
		auto matData = data["volumePosition"];
		//volumePosition = glm::mat4(matData[0][0], matData[0][1], matData[0][2], matData[0][3],
		//	matData[1][0], matData[1][1], matData[1][2], matData[1][3],
		//	matData[2][0], matData[2][1], matData[2][2], matData[2][3],
		//	matData[3][0], matData[3][1], matData[3][2], matData[3][3]);
	}
};
#endif // !RENDERER_H