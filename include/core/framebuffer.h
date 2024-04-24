#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

class Framebuffer {
public:
	Framebuffer():Framebuffer(1920, 1080) {};
	Framebuffer(int width, int height) :Framebuffer(width, height, 1) {};
	Framebuffer(int width, int height, int numberOfColorAttachments);
	unsigned int getID();
	unsigned int * getTexture();
private:
	unsigned int ID;
	unsigned int * TEXTURE;
};

#endif