#ifndef TEXTURE_H
#define TEXTURE_H

class Texture {
public:
	unsigned int ID;
	Texture(char * texturePath, unsigned int glTextureUnit, unsigned int glColorChannel);
};

#endif