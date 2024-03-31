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
#include <live-application/GUI/volumeGUI.h>
#include <vector>

// Volume Scene bündelt eine Camera mit einem volume objekt. Es ist zu verstehen wie ein koordinatensystem mit punkten
// die punkte sind die Objekte


void VolumeScene::setVolumeData(Texture3D volumeData)
{
	volume.setData(volumeData);
}

void VolumeScene::setVolumePosition(glm::mat4 modelMatrix)
{
	volume.setPosition(modelMatrix);
}

void VolumeScene::setCameraPosition(glm::mat4 viewMatrix)
{
	camera.setViewMatrix(viewMatrix);
}

void VolumeScene::setCameraRatio()
{
}

void VolumeScene::setCameraAngle()
{
}

unsigned int VolumeScene::getVolumeVAO()
{
	return volume.getVAO();
}

unsigned int VolumeScene::getVolumeData()
{
	return volume.getData();
}

glm::mat4 VolumeScene::getVolumePosition()
{
	return volume.getPosition();
}

glm::mat4 VolumeScene::getCameraView() {
	return camera.getViewMatrix();
}

glm::mat4 VolumeScene::getCameraProjection()
{
	return camera.getProjectionMatrix();
}
