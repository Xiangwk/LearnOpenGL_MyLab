#version 420 core

layout(location = 0) in vec3 vPosition;

out vec3 TexCoord;

layout(std140, binding = 0) uniform Matrix
{
    mat4 view;
    mat4 projection;
};

void main()
{
	vec4 pos = projection * mat4(mat3(view)) * vec4(vPosition, 1.0f);
    gl_Position = pos.xyww;
	TexCoord = vPosition;
}