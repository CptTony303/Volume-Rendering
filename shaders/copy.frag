#version 330 core
layout(location = 0) out vec4 lastFrame;
layout(location = 1) out vec4 cv;
  
in vec2 TexCoords;

uniform sampler2D cvTexture;
uniform sampler2D lastFrameTexture;

void main()
{ 
    lastFrame = texture(lastFrameTexture, TexCoords);
    cv = texture(cvTexture, TexCoords);
}