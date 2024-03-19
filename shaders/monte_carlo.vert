#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoord;

out vec3 modelPos;
out vec2 lastFrameCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float screenSize_x;
uniform float screenSize_y;

void main()
{
    // note that we read the multiplication from right to left
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vec3 normalizedDeviceCoords = gl_Position.xyz / gl_Position.w;
    normalizedDeviceCoords = 0.5 * (normalizedDeviceCoords + 1.0);
    lastFrameCoord = normalizedDeviceCoords.xy;
    modelPos = aPos;
}
