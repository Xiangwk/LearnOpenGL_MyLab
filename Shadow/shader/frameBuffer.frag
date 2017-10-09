#version 420 core

in vec2 TexCoord;

out vec4 fragColor;

uniform sampler2D screenTexture;

void main()
{
    vec4 Color = texture(screenTexture, TexCoord);
	fragColor = vec4(vec3(Color.r), 1.0f);
}