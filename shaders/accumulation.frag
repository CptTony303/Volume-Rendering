#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D screenCopy;

uniform int samplesPerRun;
uniform int runs;

void main()
{ 
    if(runs <= 0){
        FragColor = texture(screenTexture, TexCoords);
    }else{
        FragColor = texture(screenTexture, TexCoords) + texture(screenCopy, TexCoords)*runs;
        FragColor /= runs+1;
//        FragColor = texture(screenTexture, TexCoords) + texture(screenCopy, TexCoords)*9;
//        FragColor /= 10;
    }
}