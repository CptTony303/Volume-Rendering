#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include <glm/glm.hpp>

class Texture3D {
public:
	unsigned int ID;
	Texture3D(char * texturePath, unsigned int glTextureUnit, glm::vec3 dimension);
};

#endif