#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vTexCoord;

out vec3 Color;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //let x * 0.75 to eliminate the viewport's extend transform on x-oriented
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	Color = vColor;
	TexCoord = vTexCoord;
}