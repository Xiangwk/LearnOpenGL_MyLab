#version 420 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;

out VS_OUT
{
    vec4 normalEnd;
}vs_out;

uniform mat4 model;
layout(std140, binding = 0) uniform Matrix
{
    mat4 view;
    mat4 projection;
};

const float MAGNITUDE = 0.2;

void main()
{
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	//the normal need to translate to clip-coordinate
	vs_out.normalEnd = projection * view * model * vec4(vPosition + normalize(vNormal) * MAGNITUDE, 1.0f);
}