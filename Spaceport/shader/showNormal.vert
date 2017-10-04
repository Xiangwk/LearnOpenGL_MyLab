#version 420 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;

out VS_OUT
{
    vec3 Normal;
}vs_out;

uniform mat4 model;
layout(std140, binding = 0) uniform Matrix
{
    mat4 view;
    mat4 projection;
};

void main()
{
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	vs_out.Normal = normalize(vec3(projection * vec4(normalMatrix * vNormal, 1.0f)));
}