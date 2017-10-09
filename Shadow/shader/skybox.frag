#version 420 core

in vec3 TexCoord;

out vec4 Color;

uniform samplerCube cubeMap;

void main()
{
    Color = texture(cubeMap, TexCoord);
}