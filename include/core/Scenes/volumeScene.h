#ifndef VOLUME_SCENE_H
#define VOLUME_SCENE_H


#include <core/shader.h>
#include <core/framebuffer.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <core/camera.h>
#include <core/GUI/volumeGUI.h>
#include <vector>
#include <core/Objects/volume.h>

class VolumeScene{
public:
	VolumeScene(float imageRatio) : volume(Volume()),
		camera(Camera(imageRatio)) {};

	void setVolumeData(char* path);
	void setVolumePosition(glm::mat4 modelMatrix);

	void setCameraPosition();
	void setCameraRatio();
	void setCameraAngle();

	unsigned int getVolumeVAO();
	unsigned int getVolumeData();
	glm::mat4 getVolumePosition();

	glm::mat4 getCameraView();
	glm::mat4 getCameraProjection();


private:
	//private member variables
	Camera camera;
	Volume volume;
};

#endif