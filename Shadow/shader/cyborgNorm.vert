#version 420 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBiTangent;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out vec4 FragPosDirLightSpace;
out mat3 TBN;

uniform mat4 model;
layout(std140, binding = 0) uniform Matrix
{
    mat4 view;
    mat4 projection;
};

uniform mat4 dirLightSpaceMatrix;

void main()
{
    vec3 T = normalize(vec3(model * vec4(vTangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(vBiTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(vNormal,     0.0)));
    TBN = mat3(T, B, N);

	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	FragPos = vec3(model * vec4(vPosition, 1.0f));
	Normal = mat3(model) * vNormal;
	TexCoord = vTexCoord;
	FragPosDirLightSpace = dirLightSpaceMatrix * vec4(FragPos, 1.0f);
}