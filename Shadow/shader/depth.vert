#version 420 core

layout(location = 0) in vec3 vPosition;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(vPosition, 1.0f);
}