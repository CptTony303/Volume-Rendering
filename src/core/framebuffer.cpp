#include <core/framebuffer.h>
#include <glad/glad.h>
#include <iostream>

Framebuffer::Framebuffer(int width, int height, int numberOfColorAttachments) {

	TEXTURE = new unsigned int[numberOfColorAttachments];

	unsigned int local_id;
	glGenFramebuffers(1, &local_id);
	glBindFramebuffer(GL_FRAMEBUFFER, local_id);

	//save currently bound Texture
	/*unsigned int boundTexture;
	glGet(GL_TEXTURE_BINDING_2D);*/
	// generate texture
	for (int i = 0; i < numberOfColorAttachments; i++) {

	unsigned int texture;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, texture, 0);
	TEXTURE[i] = texture;
	}
	// generate depthbuffer
	unsigned int depthbuffer;
	glGenTextures(1, &depthbuffer);
	glBindTexture(GL_TEXTURE_2D, depthbuffer);
	//get actual screen size
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthbuffer, 0);

	ID = local_id;
	
}
unsigned int Framebuffer::getID() {
	return ID;
}

unsigned int * Framebuffer::getTexture()
{
	return TEXTURE;
}
